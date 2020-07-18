/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    path_tracer.h

Abstract:

    Creates an path tracer.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PATH_TRACER_
#define _IRIS_PHYSX_TOOLKIT_PATH_TRACER_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PathTracerAllocate(
    _In_ uint8_t min_bounces,
    _In_ uint8_t max_bounces,
    _In_ float_t min_termination_probability,
    _In_ float_t roulette_threshold,
    _Out_ PINTEGRATOR *integrator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PATH_TRACER_