/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_raytracer.h

Abstract:

    This file contains the definitions for the SAMPLER_RAYTRACER type.

--*/

#ifndef _SAMPLER_RAYTRACER_IRIS_CAMERA_
#define _SAMPLER_RAYTRACER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef struct _SAMPLE_RAYTRACER SAMPLE_RAYTRACER, *PSAMPLE_RAYTRACER;
typedef CONST SAMPLE_RAYTRACER *PCSAMPLE_RAYTRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleRayTracerTrace(
    _In_ PCSAMPLE_RAYTRACER RayTracer,
    _In_ RAY WorldRay,
    _Out_ PCOLOR3 Color
    );

#endif // _SAMPLER_RAYTRACER_IRIS_CAMERA_