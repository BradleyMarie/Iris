/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    render.c

Abstract:

    Renders an image using the camera, pixel sampler, and sampler, rng, and
    framebuffer specified.

--*/

#include <limits.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>

#include "common/pointer_list.h"
#include "iris_camera/camera_internal.h"
#include "iris_camera/framebuffer_internal.h"
#include "iris_camera/image_sampler_internal.h"
#include "iris_camera/progress_reporter_internal.h"
#include "iris_camera/render.h"
#include "iris_camera/sample_tracer_internal.h"

//
// Defines
//

#define CHUNK_SIZE 32

//
// Types
//

typedef struct _RENDER_THREAD_LOCAL_STATE {
    PSAMPLE_TRACER sample_tracer;
    PPROGRESS_REPORTER progress_reporter;
    ISTATUS status;
} RENDER_THREAD_LOCAL_STATE, *PRENDER_THREAD_LOCAL_STATE;

typedef struct _RENDER_THREAD_SHARED_STATE {
    PCCAMERA camera;
    PCMATRIX camera_to_world;
    PFRAMEBUFFER framebuffer;
    POINTER_LIST seed_rngs;
    POINTER_LIST unowned_rngs;
    POINTER_LIST image_samplers;
    float_t epsilon;
    atomic_bool cancelled;
    atomic_size_t chunk;
} RENDER_THREAD_SHARED_STATE, *PRENDER_THREAD_SHARED_STATE;

typedef const RENDER_THREAD_SHARED_STATE *PCRENDER_THREAD_SHARED_STATE;

typedef struct _RENDER_THREAD_CONTEXT {
    PRENDER_THREAD_SHARED_STATE shared;
    RENDER_THREAD_LOCAL_STATE local;
} RENDER_THREAD_CONTEXT, *PRENDER_THREAD_CONTEXT;

typedef const RENDER_THREAD_CONTEXT *PCRENDER_THREAD_CONTEXT;

//
// Static Functions
//

static
void
IrisCameraFreeThreadState(
    _In_ size_t num_threads,
    _Inout_updates_(num_threads) _Post_invalid_ PRENDER_THREAD_CONTEXT thread_state
    )
{
    assert(num_threads != 0);
    assert(thread_state != NULL);

    for (size_t i = 1; i < num_threads; i++)
    {
        SampleTracerFree(thread_state[i].local.sample_tracer);
    }

    free(thread_state);
}

static
ISTATUS
IrisCameraAllocateThreadState(
    _In_ size_t num_threads,
    _In_ PRENDER_THREAD_SHARED_STATE shared_state,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_opt_ PPROGRESS_REPORTER progress_reporter,
    _Outptr_result_buffer_(num_threads) PRENDER_THREAD_CONTEXT *thread_state
    )
{
    assert(num_threads != 0);
    assert(sample_tracer != NULL);
    assert(thread_state != NULL);

    PRENDER_THREAD_CONTEXT result =
        (PRENDER_THREAD_CONTEXT)calloc(num_threads, sizeof(RENDER_THREAD_CONTEXT));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    result[0].shared = shared_state;
    result[0].local.sample_tracer = sample_tracer;
    result[0].local.progress_reporter = progress_reporter;

    for (size_t i = 1; i < num_threads; i++)
    {
        result[i].shared = shared_state;
        result[i].local.progress_reporter = NULL;

        ISTATUS status = SampleTracerDuplicate(sample_tracer,
                                               &result[i].local.sample_tracer);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        result[i].local.status = ISTATUS_SUCCESS;
    }

    *thread_state = result;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
IrisCameraRenderPixel(
    _Inout_ PRENDER_THREAD_CONTEXT context,
    _Inout_ PIMAGE_SAMPLER image_sampler,
    _Inout_ PRANDOM rng,
    _In_ float_t pixel_u_width,
    _In_ float_t pixel_v_width,
    _In_ size_t num_columns,
    _In_ size_t num_rows,
    _In_ size_t pixel_column,
    _In_ size_t pixel_row
    )
{
    assert(context != NULL);
    assert(image_sampler != NULL);
    assert(rng != NULL);
    assert(isfinite(pixel_u_width));
    assert((float_t)0.0 < pixel_u_width);
    assert(isfinite(pixel_v_width));
    assert((float_t)0.0 > pixel_v_width);

    float_t pixel_u_min =
        context->shared->camera->image_min_u + (float_t)pixel_column * pixel_u_width;
    float_t pixel_u_max = pixel_u_min + pixel_u_width;

    float_t pixel_v_max =
        context->shared->camera->image_max_v + (float_t)pixel_row * pixel_v_width;
    float_t pixel_v_min = pixel_v_max + pixel_v_width;

    size_t in_order_column;
    if (pixel_u_width < (float_t)0.0)
    {
        in_order_column = num_columns - pixel_column - 1;
    }
    else
    {
        in_order_column = pixel_column;
    }

    size_t in_order_row;
    if (pixel_v_width < (float_t)0.0)
    {
        in_order_row = num_rows - pixel_row - 1;
    }
    else
    {
        in_order_row = pixel_row;
    }

    size_t num_samples;
    ISTATUS status =
        ImageSamplerPreparePixelSamples(image_sampler,
                                        in_order_column,
                                        in_order_row,
                                        pixel_u_min,
                                        pixel_u_max,
                                        pixel_v_min,
                                        pixel_v_max,
                                        context->shared->camera->lens_min_u,
                                        context->shared->camera->lens_max_u,
                                        context->shared->camera->lens_min_v,
                                        context->shared->camera->lens_max_v,
                                        &num_samples);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    COLOR3 pixel_color = ColorCreateBlack();

    for (size_t index = 0; index < num_samples; index++)
    {
        bool cancelled = atomic_load_explicit(&context->shared->cancelled,
                                              memory_order_relaxed);

        if (cancelled)
        {
            return ISTATUS_SUCCESS;
        }

        float_t pixel_u, pixel_v, lens_u, lens_v, dpixel_u, dpixel_v;
        status =
            ImageSamplerGetSample(image_sampler,
                                  rng,
                                  index,
                                  &pixel_u,
                                  &pixel_v,
                                  &lens_u,
                                  &lens_v,
                                  &dpixel_u,
                                  &dpixel_v);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        RAY_DIFFERENTIAL camera_ray_differential;
        status = CameraGenerateRayDifferential(context->shared->camera,
                                               pixel_u,
                                               pixel_v,
                                               lens_u,
                                               lens_v,
                                               dpixel_u,
                                               dpixel_v,
                                               &camera_ray_differential);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        RAY_DIFFERENTIAL world_ray_differential =
            RayDifferentialMatrixMultiply(context->shared->camera_to_world,
                                          camera_ray_differential);
        world_ray_differential =
            RayDifferentialNormalize(world_ray_differential);

        COLOR3 sample_color;
        status = SampleTracerTrace(context->local.sample_tracer,
                                   &world_ray_differential,
                                   rng,
                                   context->shared->epsilon,
                                   &sample_color);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        pixel_color = ColorAdd(pixel_color,
                               sample_color,
                               sample_color.color_space);
    }

    if (num_samples != 0)
    {
        float sample_weight = 1.0f / (float)num_samples;
        pixel_color = ColorScale(pixel_color, sample_weight);
    }

    FramebufferSetPixel(context->shared->framebuffer,
                        pixel_column,
                        pixel_row,
                        pixel_color);

    return ISTATUS_SUCCESS;
}

static
int
IrisCameraRenderThread(
    _Inout_ void *context
    )
{
    PRENDER_THREAD_CONTEXT thread_context = (PRENDER_THREAD_CONTEXT)context;
    PPROGRESS_REPORTER progress_reporter =
        thread_context->local.progress_reporter;

    size_t num_columns, num_rows;
    FramebufferGetSize(thread_context->shared->framebuffer,
                       &num_columns,
                       &num_rows);

    size_t num_chunks = num_columns / CHUNK_SIZE;
    if (num_columns % CHUNK_SIZE != 0)
    {
        num_chunks += 1;
    }

    num_chunks *= num_rows;

    size_t num_pixels = num_rows * num_columns;
    size_t pixels_per_chunk = num_pixels / num_chunks;

    float_t image_u_width = thread_context->shared->camera->image_max_u -
                            thread_context->shared->camera->image_min_u;
    float_t pixel_u_width = image_u_width / (float_t)num_columns;

    float_t image_v_width = thread_context->shared->camera->image_min_v -
                            thread_context->shared->camera->image_max_v;
    float_t pixel_v_width = image_v_width / (float_t)num_rows;

    size_t chunk =
        atomic_load_explicit(&thread_context->shared->chunk,
                             memory_order_relaxed);

    while (chunk < num_chunks)
    {
        bool success =
            atomic_compare_exchange_weak(&thread_context->shared->chunk,
                                         &chunk,
                                         chunk + 1);

        if (!success)
        {
            continue;
        }

        size_t row = chunk % num_rows;
        size_t column_chunk = chunk / num_rows;
        size_t column_base = column_chunk * CHUNK_SIZE;
        for (size_t column = column_base;
             column < column_base + CHUNK_SIZE && column < num_columns;
             column++)
        {
            PIMAGE_SAMPLER image_sampler =
                PointerListRetrieveAtIndex(&thread_context->shared->image_samplers,
                                           chunk);

            PRANDOM rng =
                PointerListRetrieveAtIndex(&thread_context->shared->unowned_rngs,
                                           chunk);

            ISTATUS status = IrisCameraRenderPixel(thread_context,
                                                   image_sampler,
                                                   rng,
                                                   pixel_u_width,
                                                   pixel_v_width,
                                                   num_columns,
                                                   num_rows,
                                                   column,
                                                   row);

            if (status != ISTATUS_SUCCESS)
            {
                atomic_store(&thread_context->shared->cancelled, true);
                thread_context->local.status = status;
                return 0;
            }
        }

        if (progress_reporter != NULL)
        {
            ISTATUS status = ProgressReporterReport(progress_reporter,
                                                    num_pixels,
                                                    chunk * pixels_per_chunk);

            if (status != ISTATUS_SUCCESS)
            {
                atomic_store(&thread_context->shared->cancelled, true);
                thread_context->local.status = status;
                return 0;
            }
        }

        chunk =
            atomic_load_explicit(&thread_context->shared->chunk,
                                 memory_order_relaxed);
    }

    return 0;
}

void
IrisCameraFreeImageSamplers(
    _Inout_ PPOINTER_LIST image_sampler_list
    )
{
    assert(image_sampler_list != NULL);

    size_t num_image_samplers = PointerListGetSize(image_sampler_list);

    for (size_t i = 0; i < num_image_samplers; i++)
    {
        PIMAGE_SAMPLER is = PointerListRetrieveAtIndex(image_sampler_list, i);
        ImageSamplerFree(is);
    }
}

void
IrisCameraFreeSeedRngs(
    _Inout_ PPOINTER_LIST seed_rng_list
    )
{
    assert(seed_rng_list != NULL);

    size_t num_seed_rngs = PointerListGetSize(seed_rng_list);

    for (size_t i = 0; i < num_seed_rngs; i++)
    {
        PRANDOM rng = PointerListRetrieveAtIndex(seed_rng_list, i);
        RandomFree(rng);
    }
}

//
// Functions
//

ISTATUS
IrisCameraRender(
    _In_ PCCAMERA camera,
    _In_opt_ PCMATRIX camera_to_world,
    _Inout_ PIMAGE_SAMPLER image_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer,
    _Inout_opt_ PPROGRESS_REPORTER progress_reporter,
    _In_ float_t epsilon,
    _In_ size_t number_of_threads
    )
{
    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (image_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(epsilon) || epsilon < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (number_of_threads < 1)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    thrd_t *threads = NULL;
    if (number_of_threads > 1)
    {
        threads = calloc(number_of_threads - 1, sizeof(thrd_t));

        if (threads == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    size_t num_columns, num_rows;
    FramebufferGetSize(framebuffer,
                       &num_columns,
                       &num_rows);

    size_t num_pixels = num_rows * num_columns;

    ISTATUS status = ImageSamplerPrepareImageSamples(image_sampler,
                                                     num_columns,
                                                     num_rows);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    RENDER_THREAD_SHARED_STATE shared_state;
    shared_state.camera = camera;
    shared_state.camera_to_world = camera_to_world;
    shared_state.framebuffer = framebuffer;
    shared_state.epsilon = epsilon;
    shared_state.cancelled = false;
    shared_state.chunk = 0;

    bool success = PointerListInitialize(&shared_state.seed_rngs);

    if (!success)
    {
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = PointerListInitialize(&shared_state.image_samplers);

    if (!success)
    {
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = PointerListInitialize(&shared_state.unowned_rngs);

    if (!success)
    {
        PointerListDestroy(&shared_state.image_samplers);
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t num_chunks = num_columns / CHUNK_SIZE;
    if (num_columns % CHUNK_SIZE != 0)
    {
        num_chunks += 1;
    }

    num_chunks *= num_rows;

    success = PointerListPrepareToAddPointers(&shared_state.seed_rngs, num_chunks);

    if (!success)
    {
        PointerListDestroy(&shared_state.unowned_rngs);
        PointerListDestroy(&shared_state.image_samplers);
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = PointerListPrepareToAddPointers(&shared_state.image_samplers,
                                              num_chunks);

    if (!success)
    {
        PointerListDestroy(&shared_state.unowned_rngs);
        PointerListDestroy(&shared_state.image_samplers);
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = PointerListPrepareToAddPointers(&shared_state.unowned_rngs,
                                              num_chunks);

    if (!success)
    {
        PointerListDestroy(&shared_state.unowned_rngs);
        PointerListDestroy(&shared_state.image_samplers);
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_chunks; i++)
    {
        PRANDOM replicated_rng;
        ISTATUS status = RandomReplicate(rng, &replicated_rng);

        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
            PointerListDestroy(&shared_state.image_samplers);
            PointerListDestroy(&shared_state.seed_rngs);
            free(threads);
            return status;
        }

        PointerListAddPointer(&shared_state.seed_rngs, replicated_rng);
    }

    for (size_t i = 0; i < num_chunks; i++)
    {
        PIMAGE_SAMPLER replicated_image_sampler;
        ISTATUS status = ImageSamplerReplicate(image_sampler,
                                               &replicated_image_sampler);

        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeImageSamplers(&shared_state.image_samplers);
            IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
            PointerListDestroy(&shared_state.unowned_rngs);
            PointerListDestroy(&shared_state.image_samplers);
            PointerListDestroy(&shared_state.seed_rngs);
            free(threads);
            return status;
        }

        PointerListAddPointer(&shared_state.image_samplers,
                              replicated_image_sampler);
    }

    for (size_t i = 0; i < num_chunks; i++)
    {
        PRANDOM seed_rng =
            PointerListRetrieveAtIndex(&shared_state.seed_rngs, i);

        PRANDOM unowned_rng;
        ISTATUS status = ImageSamplerPrepareRandom(image_sampler,
                                                   seed_rng,
                                                   &unowned_rng);

        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeImageSamplers(&shared_state.image_samplers);
            IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
            PointerListDestroy(&shared_state.unowned_rngs);
            PointerListDestroy(&shared_state.image_samplers);
            PointerListDestroy(&shared_state.seed_rngs);
            free(threads);
            return status;
        }

        PointerListAddPointer(&shared_state.unowned_rngs,
                              unowned_rng);
    }

    PRENDER_THREAD_CONTEXT thread_contexts;
    status = IrisCameraAllocateThreadState(number_of_threads,
                                           &shared_state,
                                           sample_tracer,
                                           progress_reporter,
                                           &thread_contexts);

    if (status != ISTATUS_SUCCESS)
    {
        IrisCameraFreeImageSamplers(&shared_state.image_samplers);
        IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
        PointerListDestroy(&shared_state.unowned_rngs);
        PointerListDestroy(&shared_state.image_samplers);
        PointerListDestroy(&shared_state.seed_rngs);
        free(threads);
        return status;
    }

    if (progress_reporter != NULL)
    {
        status = ProgressReporterReport(progress_reporter, num_pixels, 0);

        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(number_of_threads, thread_contexts);
            IrisCameraFreeImageSamplers(&shared_state.image_samplers);
            IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
            PointerListDestroy(&shared_state.unowned_rngs);
            PointerListDestroy(&shared_state.image_samplers);
            PointerListDestroy(&shared_state.seed_rngs);
            free(threads);
            return status;
        }
    }

    size_t threads_started = 0;
    for (size_t i = 0; i < number_of_threads - 1; i++)
    {
        int success = thrd_create(threads + i,
                                  IrisCameraRenderThread,
                                  thread_contexts + i + 1);

        if (success != ISTATUS_SUCCESS)
        {
            atomic_store(&shared_state.cancelled, true);
            thread_contexts[0].local.status = ISTATUS_ALLOCATION_FAILED;
            break;
        }

        threads_started += 1;
    }

    IrisCameraRenderThread(thread_contexts);

    status = ISTATUS_SUCCESS;
    for (size_t i = 0; i < threads_started; i++)
    {
        int success = thrd_join(threads[i], NULL);

        if (success != thrd_success)
        {
            status = ISTATUS_INVALID_RESULT;
        }
    }

    for (size_t i = 0; i < number_of_threads; i++)
    {
        if (thread_contexts[i].local.status == ISTATUS_SUCCESS)
        {
            status = thread_contexts[i].local.status;
            break;
        }
    }

    IrisCameraFreeThreadState(number_of_threads, thread_contexts);
    IrisCameraFreeImageSamplers(&shared_state.image_samplers);
    IrisCameraFreeSeedRngs(&shared_state.seed_rngs);
    PointerListDestroy(&shared_state.unowned_rngs);
    PointerListDestroy(&shared_state.image_samplers);
    PointerListDestroy(&shared_state.seed_rngs);
    free(threads);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (progress_reporter != NULL)
    {
        status = ProgressReporterReport(progress_reporter,
                                        num_pixels,
                                        num_pixels);
    }

    return status;
}

ISTATUS
IrisCameraRenderSingleThreaded(
    _In_ PCCAMERA camera,
    _In_opt_ PCMATRIX camera_to_world,
    _Inout_ PIMAGE_SAMPLER image_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer,
    _Inout_opt_ PPROGRESS_REPORTER progress_reporter,
    _In_ float_t epsilon
    )
{
    ISTATUS status = IrisCameraRender(camera,
                                      camera_to_world,
                                      image_sampler,
                                      sample_tracer,
                                      rng,
                                      framebuffer,
                                      progress_reporter,
                                      epsilon,
                                      1);

    return status;
}