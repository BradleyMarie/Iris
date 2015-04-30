/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    reflector.c

Abstract:

    This file contains the definitions for the REFLECTOR type.

--*/

#include <irisphysxp.h>

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

_Check_return_
_Ret_maybenull_
PREFLECTOR
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE ReflectorVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PREFLECTOR Reflector;

    if (ReflectorVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(REFLECTOR),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Reflector = (PREFLECTOR) HeaderAllocation;

    Reflector->VTable = ReflectorVTable;
    Reflector->Data = DataAllocation;
    Reflector->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Reflector;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorReflect(
    _In_ PCREFLECTOR Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    ISTATUS Status;

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(Wavelength) == FALSE ||
        IsFiniteFloat(Wavelength) == FALSE ||
        IsPositiveFloat(Wavelength) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (IsNormalFloat(IncomingIntensity) == FALSE ||
        IsFiniteFloat(IncomingIntensity) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    
    if (OutgoingIntensity == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = Reflector->VTable->ReflectRoutine(Reflector->Data,
                                               Wavelength,
                                               IncomingIntensity,
                                               OutgoingIntensity);

    return Status;
}

VOID
ReflectorReference(
    _In_opt_ PREFLECTOR Reflector
    )
{
    if (Reflector == NULL)
    {
        return;
    }

    Reflector->ReferenceCount += 1;
}

VOID
ReflectorDereference(
    _In_opt_ _Post_invalid_ PREFLECTOR Reflector
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Reflector == NULL)
    {
        return;
    }

    Reflector->ReferenceCount -= 1;

    if (Reflector->ReferenceCount == 0)
    {
        FreeRoutine = Reflector->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Reflector->Data);
        }

        IrisAlignedFree(Reflector);
    }
}