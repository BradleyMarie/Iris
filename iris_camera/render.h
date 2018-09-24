/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    render.h

Abstract:

    Renders an image using the camera, pixel sampler, and sampler, rng, and
    framebuffer specified.

--*/

#ifndef _IRIS_CAMERA_RENDER_
#define _IRIS_CAMERA_RENDER_

#include "iris_camera/camera.h"
#include "iris_camera/framebuffer.h"
#include "iris_camera/pixel_sampler.h"
#include "iris_camera/random_generator.h"
#include "iris_camera/sample_tracer_generator.h"

//
// Functions
//

ISTATUS
IrisCameraRender(
    _In_ PCCAMERA camera,
    _In_ PCPIXEL_SAMPLER pixel_sampler,
    _In_ PCSAMPLE_TRACER sample_tracer,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer
    );

ISTATUS
IrisCameraRenderParallel(
    _In_ PCCAMERA camera,
    _In_ PCPIXEL_SAMPLER pixel_sampler,
    _In_ PCSAMPLE_TRACER_GENERATOR sample_tracer_generator,
    _In_ PCRANDOM_GENERATOR rng_generator,
    _Inout_ PFRAMEBUFFER framebuffer
    );

#endif // _RENDER_IRIS_CAMERA_