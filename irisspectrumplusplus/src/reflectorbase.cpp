/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    reflectorbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ ReflectorBase type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorBase::Reflect(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    assert(Context != nullptr);
    assert(IsFiniteFloat(Wavelength) != FALSE);
    assert(IsGreaterThanOrEqualToZeroFloat(Wavelength) != FALSE);
    assert(IsFiniteFloat(IncomingIntensity) != FALSE);
    assert(IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) != FALSE);
    assert(OutgoingIntensity != nullptr);
    
    const ReflectorBase **ReflectorBasePtr = (const ReflectorBase**) Context;
    *OutgoingIntensity = (*ReflectorBasePtr)->Reflect(Wavelength, IncomingIntensity);
    return ISTATUS_SUCCESS;
}

VOID 
ReflectorBase::Free(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    ReflectorBase **ReflectorBasePtr = (ReflectorBase**) Context;
    delete *ReflectorBasePtr;
}

Reflector
ReflectorBase::Create(
    _In_ std::unique_ptr<ReflectorBase> ReflectorBasePtr
    )
{
    if (!ReflectorBasePtr)
    {
        throw std::invalid_argument("ReflectorBasePtr");
    }
    
    ReflectorBase *UnmanagedReflectorBasePtr = ReflectorBasePtr.release();
    PREFLECTOR ReflectorPtr;

    ISTATUS Success = ReflectorAllocate(&InteropVTable,
                                        &UnmanagedReflectorBasePtr,
                                        sizeof(ReflectorBase*),
                                        alignof(ReflectorBase*),
                                        &ReflectorPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Reflector(ReflectorPtr, false);
}

//
// Static Variables
//

const REFLECTOR_VTABLE ReflectorBase::InteropVTable = {
    ReflectorBase::Reflect, 
    ReflectorBase::Free
};

} // namespace IrisSpectrum