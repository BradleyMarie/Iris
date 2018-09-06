/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    render.h

Abstract:

    Renders an image using the camera, pixel sampler, and sampler specified.

--*/

#ifndef _IRIS_CAMERA_RENDER_
#define _IRIS_CAMERA_RENDER_

#include "iris_camera/camera.h"
#include "iris_camera/framebuffer.h"
#include "iris_camera/pixel_sampler.h"
#include "iris_camera/sample_tracer.h"

//
// Types
//

ISTATUS
(*PRENDER_IMAGE_ROUTINE)(
    _In_ void *context,
    _In_ PCFRAMEBUFFER framebuffer
    );

//
// Functions
//

ISTATUS
IrisCameraRender(
    _In_ PCCAMERA camera,
    _In_ PCPIXEL_SAMPLER pixel_sampler,
    _In_ PCSAMPLE_TRACER sample_tracer,
    _In_ PRANDOM rng,
    _In_ size_t image_columns,
    _In_ size_t image_rows,
    _In_ PRENDER_IMAGE_ROUTINE render_image_routine,
    _In_ void *render_image_context
    );

#endif // _RENDER_IRIS_CAMERA_