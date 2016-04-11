/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_spectrumreference.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ SpectrumReference type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _SPECTRUM_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_
#define _SPECTRUM_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class SpectrumReference final {
public:
    SpectrumReference(
        _In_opt_ PCSPECTRUM SpectrumPtr
        )
    { }
        
    _Ret_opt_
    PCSPECTRUM
    AsPCSPECTRUM(
        void
        ) const
    {
        return Data;
    }
    
    _Ret_
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const
    {
        FLOAT Result;
        
        ISTATUS Status = SpectrumSample(Data,
                                        Wavelength,
                                        &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return Result;
    }

    SpectrumReference(
        _In_ const SpectrumReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }

    SpectrumReference & 
    operator=(
        _In_ const SpectrumReference & ToCopy
        )
    {
        Data = ToCopy.Data;
        return *this;
    }

private:
    PCSPECTRUM Data;
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_