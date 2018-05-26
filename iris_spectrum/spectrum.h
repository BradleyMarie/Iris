/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum.h

Abstract:

    Interface representing a spectrum of light.

--*/

#ifndef _IRIS_SPECTRUM_SPECTRUM_
#define _IRIS_SPECTRUM_SPECTRUM_

#include "iris_spectrum/spectrum_vtable.h"

//
// Types
//

typedef struct _SPECTRUM SPECTRUM, *PSPECTRUM;
typedef const SPECTRUM *PCSPECTRUM;

//
// Functions
//

//IRISSPECTRUMAPI
ISTATUS
SpectrumAllocate(
    _In_ PCSPECTRUM_VTABLE vtable,
    _When_(data_size != 0, _In_reads_bytes_opt_(data_size)) const void *data,
    _In_ size_t data_size,
    _When_(data_size != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _Out_ PSPECTRUM *spectrum
    );

//IRISSPECTRUMAPI
ISTATUS
SpectrumSample(
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    );

//IRISSPECTRUMAPI
void
SpectrumRetain(
    _In_opt_ PSPECTRUM spectrum
    );

//IRISSPECTRUMAPI
void
SpectrumRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM spectrum
    );

#endif // _IRIS_SPECTRUM_SPECTRUM_