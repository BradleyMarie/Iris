/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_reflector.h

Abstract:

    This file contains the internal definitions for the REFLECTOR_REFERENCE type.

--*/

#ifndef _REFLECTOR_IRIS_SPECTRUM_INTERNAL_
#define _REFLECTOR_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Macros
//

#ifdef _IRIS_SPECTRUM_EXPORT_REFLECTOR_ROUTINES_
#define ReflectorReflect(Reflector,Wavelength,IncomingIntensity,OutgoingIntensity) \
        StaticReflectorReflect(Reflector,Wavelength,IncomingIntensity,OutgoingIntensity)
#endif

//
// Types
//

struct _REFLECTOR {
    PCREFLECTOR_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
ReflectorInitialize(
    _In_ PCREFLECTOR_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PREFLECTOR Reflector
    )
{
    ASSERT(VTable != NULL);
    ASSERT(Reflector != NULL);
    
    Reflector->VTable = VTable;
    Reflector->ReferenceCount = 1;
    Reflector->Data = Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ReflectorReflect(
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    ISTATUS Status;

    if (IsFiniteFloat(Wavelength) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Wavelength) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (IsFiniteFloat(IncomingIntensity) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (OutgoingIntensity == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        *OutgoingIntensity = (FLOAT) 0.0f;
        return ISTATUS_SUCCESS;
    }

    Status = Reflector->VTable->ReflectRoutine(Reflector->Data,
                                               Wavelength,
                                               IncomingIntensity,
                                               OutgoingIntensity);

    return Status;
}

#ifdef _IRIS_SPECTRUM_EXPORT_REFLECTOR_ROUTINES_
#undef ReflectorReflect
#endif

#endif // _REFLECTOR_IRIS_SPECTRUM_INTERNAL_