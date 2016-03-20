/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflector.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ Reflector type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _REFLECTOR_IRIS_SPECTRUM_PLUS_PLUS_
#define _REFLECTOR_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class Reflector final {
public:
    Reflector(
        _In_ PREFLECTOR ReflectorPtr,
        _In_ bool Retain
        )
    : Data(ReflectorPtr)
    { 
        if (Retain)
        {
            ReflectorRetain(ReflectorPtr);
        }
    }
        
    _Ret_
    PREFLECTOR
    AsPREFLECTOR(
        void
        )
    {
        return Data;
    }
        
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

    Reflector(
        _In_ const Reflector & ToCopy
        )
    : Data(ToCopy.Data)
    {
        ReflectorRetain(Data);
    }

    Reflector(
        _In_ Reflector && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    IRISSPECTRUMPLUSPLUSAPI
    Reflector & 
    operator=(
        _In_ const Reflector & ToCopy
        );

    ~Reflector(
        void
        )
    { 
        ReflectorRelease(Data);
    }

private:
    PREFLECTOR Data;
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_IRIS_SPECTRUM_PLUS_PLUS_