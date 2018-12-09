/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    directional_light.h

Abstract:

    Creates a directional light.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_DIRECTIONAL_LIGHT_
#define _IRIS_PHYSX_TOOLKIT_DIRECTIONAL_LIGHT_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
DirectionalLightAllocate(
    _In_ VECTOR3 to_light,
    _In_ PSPECTRUM spectrum,
    _Out_ PLIGHT *light
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_DIRECTIONAL_LIGHT_