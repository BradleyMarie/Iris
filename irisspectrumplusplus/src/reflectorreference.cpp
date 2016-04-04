/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    reflectorreference.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ ReflectorReference type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

ReflectorReference::ReflectorReference(
    _In_ PCREFLECTOR ReflectorPtr
    )
: Data(ReflectorPtr)
{ 
    if (ReflectorPtr == nullptr)
    {
        throw std::invalid_argument("ReflectorPtr");
    }
}

_Ret_
FLOAT
ReflectorReference::Reflect(
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity
    ) const
{
    FLOAT Result;
    
    ISTATUS Status = ReflectorReflect(Data,
                                      Wavelength,
                                      IncomingIntensity,
                                      &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Wavelength");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("IncomingIntensity");
            break;
        default:
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
            break;
    }
    
    return Result;
}

} // namespace IrisSpectrum