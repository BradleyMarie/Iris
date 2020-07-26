/*++

Copyright (c) 2020 Brad Weinberger

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
#include "iris_camera/image_sampler.h"
#include "iris_camera/progress_reporter.h"
#include "iris_camera/sample_tracer.h"

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
    );

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
    );

#endif // _RENDER_IRIS_CAMERA_