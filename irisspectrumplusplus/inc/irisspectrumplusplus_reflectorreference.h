/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflectorreference.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ ReflectorReference type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _REFLECTOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_
#define _REFLECTOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class ReflectorReference final {
public:
    ReflectorReference(
        _In_opt_ PCREFLECTOR ReflectorPtr
        )
    : Data(ReflectorPtr)
    { }
        
    _Ret_opt_
    PCREFLECTOR
    AsPCREFLECTOR(
        void
        ) const
    {
        return Data;
    }
    
    _Ret_opt_
    IRISSPECTRUMPLUSPLUSAPI
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const
    {
        FLOAT Result;
        
        ISTATUS Status = ReflectorReflect(Data,
                                          Wavelength,
                                          IncomingIntensity,
                                          &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return Result;
    }

    ReflectorReference(
        _In_ const ReflectorReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }

    ReflectorReference & 
    operator=(
        _In_ const ReflectorReference & ToCopy
        )
    {
        Data = ToCopy.Data;
        return *this;
    }

private:
    PCREFLECTOR Data;
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_