/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    render.c

Abstract:

    Renders an image using the camera, pixel sampler, and sampler, rng, and
    framebuffer specified.

--*/

#include <limits.h>
#include <omp.h>
#include <stdlib.h>

#include "iris_camera/camera_internal.h"
#include "iris_camera/framebuffer_internal.h"
#include "iris_camera/pixel_sampler_generator_internal.h"
#include "iris_camera/pixel_sampler_internal.h"
#include "iris_camera/random_generator_internal.h"
#include "iris_camera/render.h"
#include "iris_camera/sample_tracer_generator_internal.h"
#include "iris_camera/sample_tracer_internal.h"

//
// Types
//

typedef struct _RENDER_THREAD_STATE {
    PPIXEL_SAMPLER pixel_sampler;
    PSAMPLE_TRACER sample_tracer;
    PRANDOM rng;
    ISTATUS status;
} RENDER_THREAD_STATE, *PRENDER_THREAD_STATE;

//
// Static Functions
//

static
void
IrisCameraFreeThreadState(
    _In_ size_t num_threads,
    _Inout_updates_(num_threads) _Post_invalid_ PRENDER_THREAD_STATE thread_state
    )
{
    for (size_t i = 0; i < num_threads; i++)
    {
        PixelSamplerFree(thread_state[i].pixel_sampler);
        SampleTracerFree(thread_state[i].sample_tracer);
        RandomFree(thread_state[i].rng);
    }

    free(thread_state);
}

static
ISTATUS
IrisCameraAllocateThreadState(
    _In_ size_t num_threads,
    _Inout_ PPIXEL_SAMPLER_GENERATOR pixel_sampler_generator,
    _Inout_ PSAMPLE_TRACER_GENERATOR sample_tracer_generator,
    _Inout_ PRANDOM_GENERATOR rng_generator,
    _Outptr_result_buffer_(num_threads) PRENDER_THREAD_STATE *thread_state
    )
{
    assert(num_threads != 0);

    PRENDER_THREAD_STATE result = 
        (PRENDER_THREAD_STATE)calloc(num_threads, sizeof(RENDER_THREAD_STATE));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        ISTATUS status = PixelSamplerGeneratorGenerate(
            pixel_sampler_generator, &result[i].pixel_sampler);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        status = SampleTracerGeneratorGenerate(sample_tracer_generator,
                                               &result[i].sample_tracer);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        status = RandomGeneratorGenerate(rng_generator, &result[i].rng);
        
        if (status != ISTATUS_SUCCESS)
        {
            IrisCameraFreeThreadState(num_threads, result);
            return status;
        }

        result[i].status = ISTATUS_SUCCESS;
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
    _In_ float_t image_min_u,
    _In_ float_t image_max_u,
    _In_ float_t image_min_v,
    _In_ float_t image_max_v,
    _In_ size_t pixel_column,
    _In_ size_t pixel_row
    )
{
    assert(isfinite(epsilon) && (float_t)0.0 <= epsilon);
    assert(camera != NULL);
    assert(pixel_sampler != NULL);
    assert(sample_tracer != NULL);
    assert(rng != NULL);
    assert(framebuffer != NULL);
    assert(isfinite(image_min_u));
    assert(isfinite(image_max_u));
    assert(isfinite(image_min_v));
    assert(isfinite(image_max_v));
    assert(image_min_u <= image_max_u);
    assert(image_min_v <= image_max_v);

    ISTATUS status = PixelSamplerPrepareSamples(pixel_sampler,
                                                rng,
                                                image_min_u,
                                                image_max_u,
                                                image_min_v,
                                                image_max_v,
                                                camera->lens_min_u,
                                                camera->lens_max_u,
                                                camera->lens_min_v,
                                                camera->lens_max_v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (;;)
    {
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

        status = SampleTracerTrace(sample_tracer, &ray, rng, epsilon);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (sampler_status == ISTATUS_DONE)
        {
            break;
        }
    }

    COLOR3 color;
    status = SampleTracerToneMap(sample_tracer, &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    FramebufferSetPixel(framebuffer, pixel_column, pixel_row, color);

    return ISTATUS_SUCCESS;
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
    if (isinf(epsilon) || isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (pixel_sampler == NULL)
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

    size_t num_columns, num_rows;
    FramebufferGetSize(framebuffer, &num_columns, &num_rows);

    float_t pixel_u_width =
        (camera->image_max_u - camera->image_min_u) / (float_t)num_columns;

    float_t pixel_v_width = 
        (camera->image_max_v - camera->image_min_v) / (float_t)num_rows;

    float_t pixel_v_max = camera->image_max_v;
    float_t pixel_v_min = pixel_v_max - pixel_v_width;

    for (size_t row = 0; row < num_rows; row++)
    {
        float_t pixel_u_min = camera->image_min_u;
        float_t pixel_u_max = pixel_u_min + pixel_u_width;

        for (size_t column = 0; column < num_columns; column++)
        {
            ISTATUS status = IrisCameraRenderPixel(epsilon,
                                                   camera,
                                                   pixel_sampler,
                                                   sample_tracer,
                                                   rng,
                                                   framebuffer,
                                                   pixel_u_min,
                                                   pixel_u_max,
                                                   pixel_v_min,
                                                   pixel_v_max,
                                                   column,
                                                   row);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            pixel_u_min = pixel_u_max;
            pixel_u_max += pixel_u_width;
        }
        
        pixel_v_max = pixel_v_min;
        pixel_v_min -= pixel_v_width;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
IrisCameraRenderParallel(
    _In_ size_t number_of_threads,
    _In_ float_t epsilon,
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER_GENERATOR pixel_sampler_generator,
    _Inout_ PSAMPLE_TRACER_GENERATOR sample_tracer_generator,
    _Inout_ PRANDOM_GENERATOR rng_generator,
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

    if (pixel_sampler_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (sample_tracer_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (rng_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    PRENDER_THREAD_STATE thread_state;
    ISTATUS status = IrisCameraAllocateThreadState(number_of_threads,
                                                   pixel_sampler_generator,
                                                   sample_tracer_generator,
                                                   rng_generator,
                                                   &thread_state);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t num_columns, num_rows;
    FramebufferGetSize(framebuffer, &num_columns, &num_rows);

    float_t pixel_u_width =
        (camera->image_max_u - camera->image_min_u) / (float_t)num_columns;

    float_t pixel_v_width = 
        (camera->image_max_v - camera->image_min_v) / (float_t)num_rows;
    
    int old_num_threads = omp_get_num_threads();
    omp_set_num_threads((int)number_of_threads);

    #pragma omp parallel
    {
    #pragma omp for schedule(dynamic, 1) private(status)
    for (size_t i = 0; i < num_columns * num_rows; i++)
    {
        size_t column = i % num_columns;
        size_t row = i % num_columns;

        float_t pixel_u_min = 
            camera->image_min_u + pixel_u_width * (float_t)num_columns;
        float_t pixel_u_max = pixel_u_min + pixel_u_width;

        float_t pixel_v_max = 
            camera->image_max_v - pixel_v_width * (float_t)row;
        float_t pixel_v_min = pixel_v_max - pixel_v_width;

        size_t thread_num = (size_t)omp_get_thread_num();
        status = IrisCameraRenderPixel(epsilon,
                                       camera,
                                       thread_state[thread_num].pixel_sampler,
                                       thread_state[thread_num].sample_tracer,
                                       thread_state[thread_num].rng,
                                       framebuffer,
                                       pixel_u_min,
                                       pixel_u_max,
                                       pixel_v_min,
                                       pixel_v_max,
                                       column,
                                       row);

        if (status != ISTATUS_SUCCESS)
        {
            thread_state[i].status = status;
            #pragma omp cancel for 
        }
    }
    }

    omp_set_num_threads(old_num_threads);

    status = ISTATUS_SUCCESS;
    for (size_t i = 0; i < number_of_threads; i++)
    {
        if (thread_state[i].status == ISTATUS_SUCCESS)
        {
            continue;
        }

        status = thread_state[i].status;

        if (thread_state[i].status == ISTATUS_ALLOCATION_FAILED)
        {
            break;
        }
    }

    IrisCameraFreeThreadState(number_of_threads, thread_state);

    return status;
}