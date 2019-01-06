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
#include "iris_camera/sample_tracer.h"

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
    );

ISTATUS
IrisCameraRenderParallel(
    _In_ size_t number_of_threads,
    _In_ float_t epsilon,
    _In_ PCCAMERA camera,
    _Inout_ PPIXEL_SAMPLER pixel_sampler,
    _Inout_ PSAMPLE_TRACER sample_tracer_generator,
    _Inout_ PRANDOM rng,
    _Inout_ PFRAMEBUFFER framebuffer
    );

#endif // _RENDER_IRIS_CAMERA_