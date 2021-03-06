/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum.h

Abstract:

    Interface representing a spectrum of light.

--*/

#ifndef _IRIS_PHYSX_SPECTRUM_
#define _IRIS_PHYSX_SPECTRUM_

#include <stddef.h>

#include "iris_physx/spectrum_vtable.h"

//
// Types
//

typedef struct _SPECTRUM SPECTRUM, *PSPECTRUM;
typedef const SPECTRUM *PCSPECTRUM;

//
// Functions
//

ISTATUS
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    );

void
SpectrumRetain(
    _In_opt_ PSPECTRUM spectrum
    );

void
SpectrumRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM spectrum
    );

#endif // _IRIS_PHYSX_SPECTRUM_