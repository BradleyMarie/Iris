/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator_vtable.h

Abstract:

    The vtable for a color integrator.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_VTABLE_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/reflector.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef
ISTATUS
(*PCOLOR_INTEGRATOR_COMPUTE_SPECTRUM_COLOR_ROUTINE)(
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    );

typedef
ISTATUS
(*PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE)(
    _In_opt_ PCREFLECTOR spectrum,
    _Out_ PCOLOR3 color
    );

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_VTABLE_