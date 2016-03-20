/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrumreference.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ SpectrumReference type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

_Ret_
FLOAT
SpectrumReference::Sample(
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