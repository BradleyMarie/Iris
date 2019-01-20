/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    sample_tracer.h

Abstract:

    Creates a sample tracer for tracing Iris Physx scenes.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SAMPLE_TRACER_
#define _IRIS_PHYSX_TOOLKIT_SAMPLE_TRACER_

#include "iris_camera/iris_camera.h"
#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//
 
ISTATUS
PhysxSampleTracerAllocate(
    _In_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCOLOR_MATCHER color_matcher,
    _Out_ PSAMPLE_TRACER *sample_tracer
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SAMPLE_TRACER_