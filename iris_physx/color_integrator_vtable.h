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
    _In_ const void *context,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    );

typedef
ISTATUS
(*PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE)(
    _In_ const void *context,
    _In_opt_ PCREFLECTOR spectrum,
    _Out_ PCOLOR3 color
    );

typedef struct _COLOR_INTEGRATOR_VTABLE {
    PCOLOR_INTEGRATOR_COMPUTE_SPECTRUM_COLOR_ROUTINE compute_spectrum_color_routine;
    PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE compute_reflector_color_routine;
    PFREE_ROUTINE free_routine;
} COLOR_INTEGRATOR_VTABLE, *PCOLOR_INTEGRATOR_VTABLE;

typedef const COLOR_INTEGRATOR_VTABLE *PCCOLOR_INTEGRATOR_VTABLE;

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_VTABLE_