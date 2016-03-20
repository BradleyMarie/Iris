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
        _In_ PCREFLECTOR ReflectorPtr
        )
    : Data(ReflectorPtr)
    { }
        
    _Ret_
    PCREFLECTOR
    AsPCREFLECTOR(
        void
        ) const
    {
        return Data;
    }
    
    _Ret_
    IRISSPECTRUMPLUSPLUSAPI
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

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