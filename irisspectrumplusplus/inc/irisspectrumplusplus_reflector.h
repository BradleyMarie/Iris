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
        _In_opt_ PREFLECTOR ReflectorPtr,
        _In_ bool Retain
        )
    : Data(ReflectorPtr)
    { 
        if (Retain)
        {
            ReflectorRetain(ReflectorPtr);
        }
    }
        
    _Ret_opt_
    PREFLECTOR
    AsPREFLECTOR(
        void
        )
    {
        return Data;
    }
        
    _Ret_opt_
    PCREFLECTOR
    AsPCREFLECTOR(
        void
        ) const
    {
        return Data;
    }
    
    ReflectorReference
    AsReflectorReference(
        void
        ) const
    {
        return ReflectorReference(Data);
    }

    _Ret_
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

    Reflector & 
    operator=(
        _In_ const Reflector & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            ReflectorRelease(Data);
            Data = ToCopy.Data;
            ReflectorRetain(Data);
        }

        return *this;
    }

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