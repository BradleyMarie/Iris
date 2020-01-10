/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_extrapolator_vtable.h

Abstract:

    The vtable for a color extrapolator.

--*/

#ifndef _IRIS_PHYSX_COLOR_TOOLKIT_EXTRAPOLATOR_VTABLE_
#define _IRIS_PHYSX_COLOR_TOOLKIT_EXTRAPOLATOR_VTABLE_

#include "iris_physx/reflector.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef
ISTATUS
(*PCOLOR_EXTRAPOLATOR_COMPUTE_SPECTRUM_ROUTINE)(
    _In_ const void *context,
    _In_ float_t color[3],
    _Out_ PSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PCOLOR_EXTRAPOLATOR_COMPUTE_REFLECTOR_ROUTINE)(
    _In_ const void *context,
    _In_ float_t color[3],
    _Out_ PREFLECTOR *reflector
    );

typedef struct _COLOR_EXTRAPOLATOR_VTABLE {
    PCOLOR_EXTRAPOLATOR_COMPUTE_SPECTRUM_ROUTINE compute_spectrum_routine;
    PCOLOR_EXTRAPOLATOR_COMPUTE_REFLECTOR_ROUTINE compute_reflector_routine;
    PFREE_ROUTINE free_routine;
} COLOR_EXTRAPOLATOR_VTABLE, *PCOLOR_EXTRAPOLATOR_VTABLE;

typedef const COLOR_EXTRAPOLATOR_VTABLE *PCCOLOR_EXTRAPOLATOR_VTABLE;

#endif // _IRIS_PHYSX_COLOR_TOOLKIT_EXTRAPOLATOR_VTABLE_