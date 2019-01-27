/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.h

Abstract:

    Computes the xyz color of a spectrum or a reflector.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_

#include <stddef.h>

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