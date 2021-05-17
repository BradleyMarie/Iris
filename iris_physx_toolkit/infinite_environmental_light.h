/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    infinite_environmental_light.h

Abstract:

    Creates a infinite environmental light.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_INFINITE_LIGHT_
#define _IRIS_PHYSX_TOOLKIT_INFINITE_LIGHT_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/spectrum_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
InfiniteEnvironmentalLightAllocate(
    _In_ PSPECTRUM_TEXTURE texture,
    _In_opt_ PMATRIX light_to_world,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_INFINITE_LIGHT_