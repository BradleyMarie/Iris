/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    one_light_sampler.h

Abstract:

    Creates a light sampler which samples all the lights it contains.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_ONE_LIGHT_SAMPLER_
#define _IRIS_PHYSX_TOOLKIT_ONE_LIGHT_SAMPLER_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _ONE_LIGHT_SAMPLER ONE_LIGHT_SAMPLER, *PONE_LIGHT_SAMPLER;
typedef const ONE_LIGHT_SAMPLER *PCONE_LIGHT_SAMPLER;

//
// Functions
//

ISTATUS
OneLightSamplerAllocate(
    _Out_ PONE_LIGHT_SAMPLER *light_sampler
    );

ISTATUS
OneLightSamplerAddLight(
    _Inout_ PONE_LIGHT_SAMPLER light_sampler,
    _In_ PLIGHT light
    );

void
OneLightSamplerFree(
    _In_opt_ _Post_invalid_ PONE_LIGHT_SAMPLER light_sampler
    );

//
// Callback Functions
//

ISTATUS 
OneLightSamplerNextSampleCallback(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ONE_LIGHT_SAMPLER_