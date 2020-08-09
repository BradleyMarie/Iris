/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    reflective_color_integrator.h

Abstract:

    An adapter for color integrators to workaround an errata in PBRT where
    colors are always calculated using the reflective color matching function.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_REFLECTIVE_COLOR_INTEGRATOR_
#define _IRIS_PHYSX_TOOLKIT_REFLECTIVE_COLOR_INTEGRATOR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ReflectiveColorIntegratorAllocate(
    _In_ PCOLOR_INTEGRATOR base_color_integrator,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_REFLECTIVE_COLOR_INTEGRATOR_