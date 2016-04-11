/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_spectrum.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ Spectrum type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _SPECTRUM_IRIS_SPECTRUM_PLUS_PLUS_
#define _SPECTRUM_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class Spectrum final {
public:
    Spectrum(
        _In_opt_ PSPECTRUM SpectrumPtr,
        _In_ bool Retain
        )
    : Data(SpectrumPtr)
    {
        if (Retain)
        {
            SpectrumRetain(SpectrumPtr);
        }
    }
        
    _Ret_opt_
    PSPECTRUM
    AsPSPECTRUM(
        void
        )
    {
        return Data;
    }
        
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

    Spectrum(
        _In_ const Spectrum & ToCopy
        )
    : Data(ToCopy.Data)
    {
        SpectrumRetain(Data);
    }

    Spectrum(
        _In_ Spectrum && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    Spectrum & 
    operator=(
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

    ~Spectrum(
        void
        )
    { 
        SpectrumRelease(Data);
    }

private:
    PSPECTRUM Data;
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_IRIS_SPECTRUM_PLUS_PLUS_