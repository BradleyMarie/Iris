/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    aggregate_environmental_light.h

Abstract:

    Creates a aggregate environmental light.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_ENVIRONMENTAL_LIGHT_
#define _IRIS_PHYSX_TOOLKIT_ENVIRONMENTAL_LIGHT_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
AggregateEnvironmentalLightAllocate(
    _In_reads_(num_lights) PENVIRONMENTAL_LIGHT *lights,
    _In_ size_t num_lights,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ENVIRONMENTAL_LIGHT_