/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrum.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ Spectrum type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

Spectrum & 
Spectrum::operator=(
    _In_ const Spectrum & ToCopy
    )
{
    if (this != &ToCopy)
    {
        SpectrumRelease(Data);
        Data = ToCopy.Data;
        SpectrumRetain(Data);
    }

    return *this;
}

_Ret_
FLOAT
Spectrum::Sample(
    _In_ FLOAT Wavelength
    ) const
{
    FLOAT Result;
    
    ISTATUS Status = SpectrumSample(Data,
                                    Wavelength,
                                    &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Wavelength");
            break;
        default:
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
            break;
    }
    
    return Result;
}

} // namespace IrisSpectrum