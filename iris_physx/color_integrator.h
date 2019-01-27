/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.h

Abstract:

    Computes the xyz color of a spectrum or a reflector.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_

#include "iris_physx/color_integrator_vtable.h"

//
// Types
//

typedef struct _COLOR_INTEGRATOR COLOR_INTEGRATOR, *PCOLOR_INTEGRATOR;
typedef const COLOR_INTEGRATOR *PCCOLOR_INTEGRATOR;

//
// Functions
//

ISTATUS
ColorIntegratorAllocate(
    _In_ PCOLOR_INTEGRATOR_COMPUTE_SPECTRUM_COLOR_ROUTINE compute_spectrum_color_routine,
    _In_ PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE compute_reflector_color_routine,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

ISTATUS
ColorIntegratorPrecomputeSpectrumColor(
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_opt_ PSPECTRUM spectrum
    );

ISTATUS
ColorIntegratorPrecomputeReflectorColor(
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_opt_ PREFLECTOR reflector
    );

void
ColorIntegratorFree(
    _In_opt_ _Post_invalid_ PCOLOR_INTEGRATOR color_integrator
    );

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_