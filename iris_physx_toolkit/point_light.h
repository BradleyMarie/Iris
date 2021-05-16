/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    point_light.h

Abstract:

    Creates a point light.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_POINT_LIGHT_
#define _IRIS_PHYSX_TOOLKIT_POINT_LIGHT_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PointLightAllocate(
    _In_ POINT3 location,
    _In_ PSPECTRUM spectrum,
    _Out_ PLIGHT *light
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_POINT_LIGHT_