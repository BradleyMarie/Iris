/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    physx_sample_tracer.h

Abstract:

    Creates a sample tracer for tracing Iris Physx scenes.

--*/

#ifndef _IRIS_PHYSX_CAMERA_TOOLKIT_PHYSX_SAMPLE_TRACER_
#define _IRIS_PHYSX_CAMERA_TOOLKIT_PHYSX_SAMPLE_TRACER_

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
    _In_opt_ PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine,
    _In_ PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine,
    _Inout_opt_ void* light_sampler_context,
    _In_ PTONE_MAPPER tone_mapper,
    _Out_ PSAMPLE_TRACER *sample_tracer
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_CAMERA_TOOLKIT_PHYSX_SAMPLE_TRACER_