/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_raytracer.h

Abstract:

    This file contains the internal definitions for the SAMPLER_RAYTRACER type.

--*/

#ifndef _RAYTRACER_IRIS_CAMERA_INTERNAL_
#define _RAYTRACER_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Types
//

struct _SAMPLE_RAYTRACER {
    PCSAMPLE_TRACER SampleTracer;
    PCVOID ThreadState;
    PRANDOM Rng;
};

//
// Functions
//

SFORCEINLINE
SAMPLE_RAYTRACER
SamplerRayTracerCreate(
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PVOID ThreadState,
    _In_ PRANDOM Rng
    )
{
    SAMPLE_RAYTRACER Result;

    ASSERT(SampleTracer != NULL);
    ASSERT(ThreadState != NULL);
    ASSERT(Rng != NULL);

    Result.SampleTracer = SampleTracer;
    Result.ThreadState = ThreadState;
    Result.Rng = Rng;

    return Result;
}

#endif // _RAYTRACER_IRIS_CAMERA_INTERNAL_