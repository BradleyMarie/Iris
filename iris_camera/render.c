/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    render.c

Abstract:

    Renders an image using the camera, pixel sampler, and sampler, rng, and
    framebuffer specified.

--*/

#include "iris_camera/camera_internal.h"
#include "iris_camera/framebuffer_internal.h"
#include "iris_camera/pixel_sampler_generator_internal.h"
#include "iris_camera/pixel_sampler_internal.h"
#include "iris_camera/random_generator_internal.h"
#include "iris_camera/render.h"
#include "iris_camera/sample_tracer_generator_internal.h"
#include "iris_camera/sample_tracer_internal.h"

//
// Static Functions
//

static
ISTATUS
IrisCameraRenderPixel(
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
        status = PixelSamplerNextSample(pixel_sampler,
                                        rng,
                                        &pixel_u,
                                        &pixel_v,
                                        &lens_u,
                                        &lens_v);
        
        if (status == ISTATUS_DONE)
        {
            break;
        }
        
        if (status != ISTATUS_SUCCESS)
        {
            return status;
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

        status = SampleTracerTrace(sample_tracer, &ray);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
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
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer
    )
{
    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (pixel_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    size_t num_columns, num_rows;
    FramebufferGetSize(framebuffer, &num_columns, &num_rows);

    float_t pixel_u_width =
        (camera->image_max_u - camera->image_min_u) / (float_t)num_columns;

    float_t pixel_v_width = 
        (camera->image_max_v - camera->image_min_v) / (float_t)num_rows;

    float_t pixel_v_max = camera->image_max_v;
    float_t pixel_v_min = pixel_v_max - pixel_v_width;

    for (size_t column = 0; column < num_columns; column++)
    {
        float_t pixel_u_min = camera->image_min_u;
        float_t pixel_u_max = pixel_u_min + pixel_u_width;

        for (size_t row = 0; row < num_rows; row++)
        {
            ISTATUS status = IrisCameraRenderPixel(camera,
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
            pixel_u_min += pixel_u_width;
        }
        
        pixel_v_max = pixel_v_min;
        pixel_v_min -= pixel_v_width;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
IrisCameraRenderParallel(
    _In_ PCCAMERA camera,
    _In_ PPIXEL_SAMPLER_GENERATOR pixel_sampler_generator,
    _In_ PCSAMPLE_TRACER_GENERATOR sample_tracer_generator,
    _In_ PCRANDOM_GENERATOR rng_generator,
    _Inout_ PFRAMEBUFFER framebuffer
    );