/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    all_light_sampler.h

Abstract:

    Creates a light sampler which samples all the lights it contains.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_ALL_LIGHT_SAMPLER_
#define _IRIS_PHYSX_TOOLKIT_ALL_LIGHT_SAMPLER_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _ALL_LIGHT_SAMPLER ALL_LIGHT_SAMPLER, *PALL_LIGHT_SAMPLER;
typedef const ALL_LIGHT_SAMPLER *PCALL_LIGHT_SAMPLER;

//
// Functions
//

ISTATUS
AllLightSamplerAllocate(
    _Out_ PALL_LIGHT_SAMPLER *light_sampler
    );

ISTATUS
AllLightSamplerAddLight(
    _Inout_ PALL_LIGHT_SAMPLER light_sampler,
    _In_ PLIGHT light
    );

void
AllLightSamplerFree(
    _In_opt_ _Post_invalid_ PALL_LIGHT_SAMPLER light_sampler
    );

//
// Callback Functions
//

ISTATUS
AllLightSamplerPrepareSamplesCallback(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point
    );

ISTATUS 
AllLightSamplerNextSampleCallback(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ALL_LIGHT_SAMPLER_