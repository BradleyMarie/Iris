/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    reflector.c

Abstract:

    This file contains the definitions for the REFLECTOR type.

--*/

#define _IRIS_SPECTRUM_EXPORT_REFLECTOR_ROUTINES_
#include <irisspectrump.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE ReflectorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PREFLECTOR *Reflector
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PREFLECTOR AllocatedReflector;

    if (ReflectorVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(REFLECTOR),
                                                      _Alignof(REFLECTOR),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedReflector = (PREFLECTOR) HeaderAllocation;

    ReflectorInitialize(ReflectorVTable,
                        DataAllocation,
                        AllocatedReflector);

    AllocatedReflector->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Reflector = AllocatedReflector;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorReflect(
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    ISTATUS Status;

    Status = StaticReflectorReflect(Reflector,
                                    Wavelength,
                                    IncomingIntensity,
                                    OutgoingIntensity);

    return Status;
}

VOID
ReflectorRetain(
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
ReflectorRelease(
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