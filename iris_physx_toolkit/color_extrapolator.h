/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    color_extrapolator.h

Abstract:

    Computes a spectrum or reflector from a color triple.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_COLOR_EXTRAPOLATOR_
#define _IRIS_PHYSX_TOOLKIT_COLOR_EXTRAPOLATOR_

#include "iris_physx_toolkit/color_extrapolator_vtable.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _COLOR_EXTRAPOLATOR COLOR_EXTRAPOLATOR, *PCOLOR_EXTRAPOLATOR;
typedef const COLOR_EXTRAPOLATOR *PCCOLOR_EXTRAPOLATOR;

//
// Functions
//

ISTATUS
ColorExtrapolatorAllocate(
    _In_ PCCOLOR_EXTRAPOLATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    );

ISTATUS
ColorExtrapolatorPrepareToComputeSpectra(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t num_spectra
    );

ISTATUS
ColorExtrapolatorPrepareToComputeReflectors(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t num_reflectors
    );

ISTATUS
ColorExtrapolatorComputeSpectrum(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
ColorExtrapolatorComputeReflector(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PREFLECTOR *reflector
    );

void
ColorExtrapolatorFree(
    _In_opt_ _Post_invalid_ PCOLOR_EXTRAPOLATOR color_extrapolator
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_COLOR_EXTRAPOLATOR_