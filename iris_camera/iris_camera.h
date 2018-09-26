/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    iris_camera.h

Abstract:

    The public header file for Iris Camera.

--*/

#ifndef _IRIS_CAMERA_IRIS_CAMERA_
#define _IRIS_CAMERA_IRIS_CAMERA_

#include "iris_advanced/iris_advanced.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

#include "iris_camera/camera.h"
#include "iris_camera/color.h"
#include "iris_camera/framebuffer.h"
#include "iris_camera/pixel_sampler.h"
#include "iris_camera/pixel_sampler_generator.h"
#include "iris_camera/random_generator.h"
#include "iris_camera/sample_tracer.h"
#include "iris_camera/sample_tracer_generator.h"

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_IRIS_CAMERA_