/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_reflectorreference.h

Abstract:

    This file contains the internal definitions for the REFLECTOR_REFERENCE type.

--*/

#ifndef _REFLECTOR_REFERENCE_IRIS_SPECTRUM_INTERNAL_
#define _REFLECTOR_REFERENCE_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Macros
//

#ifdef _IRIS_SPECTRUM_EXPORT_REFLECTOR_REFERENCE_ROUTINES_
#define ReflectorReferenceReflect(Reflector,Wavelength,IncomingIntensity,OutgoingIntensity) \
        StaticReflectorReferenceReflect(Reflector,Wavelength,IncomingIntensity,OutgoingIntensity)
#endif

//
// Types
//

struct _REFLECTOR_REFERENCE {
    PCREFLECTOR_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
ReflectorReferenceInitialize(
    _In_ PCREFLECTOR_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PREFLECTOR_REFERENCE Reflector
    )
{
    ASSERT(VTable != NULL);
    ASSERT(Reflector != NULL);
    
    Reflector->VTable = VTable;
    Reflector->Data = Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ReflectorReferenceReflect(
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
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

SFORCEINLINE
VOID
ReflectorReferenceDestroy(
    _In_ _Post_invalid_ PREFLECTOR_REFERENCE Reflector
    )
{
    PFREE_ROUTINE FreeRoutine;

    ASSERT(Reflector != NULL);

    FreeRoutine = Reflector->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Reflector->Data);
    }
}

#ifdef _IRIS_SPECTRUM_EXPORT_REFLECTOR_REFERENCE_ROUTINES_
#undef ReflectorReferenceReflect
#endif

#endif // _REFLECTOR_REFERENCE_IRIS_SPECTRUM_INTERNAL_