/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    render.c

Abstract:

    Renders an image using the camera, pixel sampler, and sampler, rng, and
    framebuffer specified.

--*/

#include <limits.h>
#include <pthread.h> // TODO: Replace with threads.h once available
#include <stdatomic.h>
#include <stdlib.h>
#include <time.h>

#include "iris_camera/camera_internal.h"
#include "iris_camera/framebuffer_internal.h"
#include "iris_camera/pixel_sampler_internal.h"
#include "iris_camera/render.h"
#include "iris_camera/sample_tracer_internal.h"

//
// Defines
//

#define MINIMUM_CHUNK_SIZE 32

//
// Types
//

typedef struct _RENDER_THREAD_LOCAL_STATE {
    PPIXEL_SAMPLER pixel_sampler;
    PSAMPLE_TRACER sample_tracer;
    PRANDOM rng;
    ISTATUS status;
} RENDER_THREAD_LOCAL_STATE, *PRENDER_THREAD_LOCAL_STATE;

typedef struct _RENDER_THREAD_SHARED_STATE {
    PCCAMERA camera;
    PFRAMEBUFFER framebuffer;
    float_t epsilon;
    atomic_bool cancelled;
    atomic_size_t pixel;
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
        PixelSamplerFree(thread_state[i].local.pixel_sampler);
        SampleTracerFree(thread_state[i].local.sample_tracer);
        RandomFree(thread_state[i].local.rng);
    }

    free(thread_state);
}

static
ISTATUS
IrisCameraAllocateThreadState(
    _In_ size_t num_threads,
    _In_ PRENDER_THREAD_SHARED_STATE shared_state,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Outptr_result_buffer_(num_threads) PRENDER_THREAD_CONTEXT *thread_state
    )
{
    assert(num_threads != 0);
    assert(pixel_sampler != NULL);
    assert(sample_tracer != NULL);
    assert(rng != NULL);
    assert(thread_state != NULL);

    PRENDER_THREAD_CONTEXT result =
        (PRENDER_THREAD_CONTEXT)calloc(num_threads, sizeof(RENDER_THREAD_CONTEXT));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    result[0].shared = shared_state;
    result[0].local.pixel_sampler = pixel_sampler;
    result[0].local.sample_tracer = sample_tracer;
    result[0].local.rng = rng;

    for (size_t i = 1; i < num_threads; i++)
    {
        result[i].shared = shared_state;

        ISTATUS status = PixelSamplerDuplicate(pixel_sampler,
                                               &result[i].local.pixel_sampler);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        status = SampleTracerDuplicate(sample_tracer,
                                       &result[i].local.sample_tracer);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        status = RandomReplicate(rng, &result[i].local.rng);
        
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
    _In_ float_t epsilon,
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer,
    _In_ float_t pixel_u_width,
    _In_ float_t pixel_v_width,
    _In_ size_t pixel_column,
    _In_ size_t pixel_row,
    _In_ atomic_bool *cancelled
    )
{
    assert(isfinite(epsilon) && (float_t)0.0 <= epsilon);
    assert(camera != NULL);
    assert(pixel_sampler != NULL);
    assert(sample_tracer != NULL);
    assert(rng != NULL);
    assert(framebuffer != NULL);
    assert(isfinite(pixel_u_width));
    assert((float_t)0.0 < pixel_u_width);
    assert(isfinite(pixel_v_width));
    assert((float_t)0.0 < pixel_v_width);
    assert(cancelled != NULL);

    float_t pixel_u_min =
        camera->image_min_u + pixel_u_width * (float_t)pixel_column;
    float_t pixel_u_max = pixel_u_min + pixel_u_width;

    float_t pixel_v_max =
        camera->image_max_v - pixel_v_width * (float_t)pixel_row;
    float_t pixel_v_min = pixel_v_max - pixel_v_width;

    ISTATUS status = PixelSamplerPrepareSamples(pixel_sampler,
                                                rng,
                                                pixel_u_min,
                                                pixel_u_max,
                                                pixel_v_min,
                                                pixel_v_max,
                                                camera->lens_min_u,
                                                camera->lens_max_u,
                                                camera->lens_min_v,
                                                camera->lens_max_v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    COLOR3 pixel_color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    size_t num_samples = 1;

    for (;;)
    {
        if (atomic_load_explicit(cancelled, memory_order_relaxed))
        {
            return ISTATUS_SUCCESS;
        }

        float_t pixel_u, pixel_v, lens_u, lens_v;
        ISTATUS sampler_status = PixelSamplerNextSample(pixel_sampler,
                                                        rng,
                                                        &pixel_u,
                                                        &pixel_v,
                                                        &lens_u,
                                                        &lens_v);
        
        if (ISTATUS_DONE < sampler_status)
        {
            return sampler_status;
        }

        RAY ray;
        status = CameraGenerateRay(camera,
                                   pixel_u,
                                   pixel_v,
                                   lens_u,
                                   lens_v,
                                   &ray);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        COLOR3 sample_color;
        status = SampleTracerTrace(sample_tracer,
                                   &ray,
                                   rng,
                                   epsilon,
                                   &sample_color);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        pixel_color = ColorAdd(pixel_color, sample_color);

        if (sampler_status == ISTATUS_DONE)
        {
            break;
        }

        num_samples += 1;
    }

    pixel_color = ColorScaleByScalar(pixel_color,
                                     (float_t)1.0 / (float_t)num_samples);

    FramebufferSetPixel(framebuffer, pixel_column, pixel_row, pixel_color);

    return ISTATUS_SUCCESS;
}

static
void*
IrisCameraRenderThread(
    _Inout_ void *context
    )
{
    PRENDER_THREAD_CONTEXT thread_context = (PRENDER_THREAD_CONTEXT)context;

    size_t num_columns, num_rows;
    FramebufferGetSize(thread_context->shared->framebuffer,
                       &num_columns,
                       &num_rows);
    size_t num_pixels = num_columns * num_rows;

    float_t image_u_width = thread_context->shared->camera->image_max_u -
                            thread_context->shared->camera->image_min_u;
    float_t pixel_u_width = image_u_width / (float_t)num_columns;

    float_t image_v_width = thread_context->shared->camera->image_max_v -
                            thread_context->shared->camera->image_min_v;
    float_t pixel_v_width = image_v_width / (float_t)num_rows;

    size_t chunk_size = MINIMUM_CHUNK_SIZE;

    size_t chunk_start =
        atomic_load_explicit(&thread_context->shared->pixel,
                             memory_order_relaxed);

    while (chunk_start < num_pixels)
    {
        size_t pixels_remaining = num_pixels - chunk_start;
        if (pixels_remaining < chunk_size)
        {
            chunk_size = pixels_remaining;
        }

        size_t chunk_end = chunk_start + chunk_size;

        bool pixels_grabbed =
            atomic_compare_exchange_weak(&thread_context->shared->pixel,
                                         &chunk_start,
                                         chunk_end);

        if (pixels_grabbed)
        {
            for (size_t i = chunk_start; i < chunk_end; i++)
            {
                size_t column = i % num_columns;
                size_t row = i / num_columns;

                ISTATUS status =
                    IrisCameraRenderPixel(thread_context->shared->epsilon,
                                          thread_context->shared->camera,
                                          thread_context->local.pixel_sampler,
                                          thread_context->local.sample_tracer,
                                          thread_context->local.rng,
                                          thread_context->shared->framebuffer,
                                          pixel_u_width,
                                          pixel_v_width,
                                          column,
                                          row,
                                          &thread_context->shared->cancelled);

                if (status != ISTATUS_SUCCESS)
                {
                    atomic_store(&thread_context->shared->cancelled, true);
                    thread_context->local.status = status;
                    return NULL;
                }
            }
        }
        else
        {
            bool cancelled =
                atomic_load_explicit(&thread_context->shared->cancelled,
                                     memory_order_relaxed);

            if (cancelled)
            {
                return NULL;
            }
        }

        chunk_start =
            atomic_load_explicit(&thread_context->shared->pixel,
                                 memory_order_relaxed);
    }

    return NULL;
}

//
// Functions
//

ISTATUS
IrisCameraRender(
    _In_ float_t epsilon,
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer
    )
{
    // TODO: Fix error codes
    ISTATUS status = IrisCameraRenderParallel(1,
                                              epsilon,
                                              camera,
                                              pixel_sampler,
                                              sample_tracer,
                                              rng,
                                              framebuffer);

    return status;
}

ISTATUS
IrisCameraRenderParallel(
    _In_ size_t number_of_threads,
    _In_ float_t epsilon,
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer
    )
{
    if (number_of_threads == 0 || INT_MAX < number_of_threads)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (isinf(epsilon) || isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (pixel_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    pthread_t *threads = NULL;
    if (number_of_threads > 1)
    {
        threads = calloc(number_of_threads - 1, sizeof(pthread_t));

        if (threads == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    RENDER_THREAD_SHARED_STATE shared_state;
    shared_state.camera = camera;
    shared_state.framebuffer = framebuffer;
    shared_state.epsilon = epsilon;
    shared_state.cancelled = false;
    shared_state.pixel = 0;

    PRENDER_THREAD_CONTEXT thread_contexts;
    ISTATUS status = IrisCameraAllocateThreadState(number_of_threads,
                                                   &shared_state,
                                                   pixel_sampler,
                                                   sample_tracer,
                                                   rng,
                                                   &thread_contexts);

    if (status != ISTATUS_SUCCESS)
    {
        free(threads);
        return status;
    }

    size_t threads_started = 0;
    for (size_t i = 0; i < number_of_threads - 1; i++)
    {
        int success = pthread_create(threads + i,
                                     NULL,
                                     IrisCameraRenderThread,
                                     thread_contexts + i + 1);

        if (success != 0)
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
        int success = pthread_join(threads[i], NULL);

        if (success != 0)
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
    free(threads);

    return status;
}