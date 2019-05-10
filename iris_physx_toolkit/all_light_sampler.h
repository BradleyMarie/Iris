/*++

Copyright (c) 2019 Brad Weinberger

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
// Functions
//

ISTATUS
AllLightSamplerAllocate(
    _In_reads_(num_lights) PLIGHT *lights,
    _In_ size_t num_lights,
    _Out_ PLIGHT_SAMPLER *light_sampler
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ALL_LIGHT_SAMPLER_