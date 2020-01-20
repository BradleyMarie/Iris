/*++

Copyright (c) 2020 Brad Weinberger

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

ISTATUS
OneLightSamplerAllocate(
    _In_reads_(num_lights) PLIGHT *lights,
    _In_ size_t num_lights,
    _Out_ PLIGHT_SAMPLER *light_sampler
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ONE_LIGHT_SAMPLER_