/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    spectrumbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ SpectrumBase type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumBase::Sample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    assert(Context != nullptr);
    assert(IsFiniteFloat(Wavelength) != FALSE);
    assert(IsGreaterThanOrEqualToZeroFloat(Wavelength) != FALSE);
    assert(OutgoingIntensity != nullptr);
    
    const SpectrumBase **SpectrumBasePtr = (const SpectrumBase**) Context;
    *OutgoingIntensity = (*SpectrumBasePtr)->Sample(Wavelength);
    return ISTATUS_SUCCESS;
}

VOID 
SpectrumBase::Free(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    SpectrumBase **SpectrumBasePtr = (SpectrumBase**) Context;
    delete *SpectrumBasePtr;
}

Spectrum
SpectrumBase::Create(
    _In_ std::unique_ptr<SpectrumBase> SpectrumBasePtr
    )
{
    if (!SpectrumBasePtr)
    {
        throw std::invalid_argument("SpectrumBasePtr");
    }
    
    SpectrumBase *UnmanagedSpectrumBasePtr = SpectrumBasePtr.release();
    PSPECTRUM SpectrumPtr;

    ISTATUS Success = SpectrumAllocate(&InteropVTable,
                                       &UnmanagedSpectrumBasePtr,
                                       sizeof(SpectrumBase*),
                                       alignof(SpectrumBase*),
                                       &SpectrumPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Spectrum(SpectrumPtr, false);
}

//
// Static Variables
//

const SPECTRUM_VTABLE SpectrumBase::InteropVTable = {
    SpectrumBase::Sample, 
    SpectrumBase::Free
};

} // namespace IrisSpectrum