/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    cie_color_integrator.h

Abstract:

    Creates a color integrator which generates a color using the CIE 1931 two
    degree standard observer sampled once per nanometer.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CIE_COLOR_INTEGRATOR_
#define _IRIS_PHYSX_TOOLKIT_CIE_COLOR_INTEGRATOR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
CieColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CIE_COLOR_INTEGRATOR_