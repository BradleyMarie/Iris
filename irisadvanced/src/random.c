/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    random.c

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#include <irisadvancedp.h>

//
// Types
//

struct _RANDOM {
    PCRANDOM_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM *Rng
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PRANDOM AllocatedRng;

    if (RandomVTable == NULL)
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

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(RANDOM),
                                                      _Alignof(RANDOM),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedRng = (PRANDOM) HeaderAllocation;

    AllocatedRng->VTable = RandomVTable;
    AllocatedRng->Data = DataAllocation;
    AllocatedRng->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}

ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    )
{
    ISTATUS Status;

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(Minimum) == FALSE ||
        IsFiniteFloat(Minimum) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsNormalFloat(Maximum) == FALSE ||
        IsFiniteFloat(Maximum) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Minimum > Maximum)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (RandomValue == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = Rng->VTable->GenerateFloatRoutine(Rng->Data,
                                               Minimum,
                                               Maximum,
                                               RandomValue);

    return Status;
}

ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    )
{
    ISTATUS Status;

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Minimum > Maximum)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (RandomValue == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = Rng->VTable->GenerateIndexRoutine(Rng->Data,
                                               Minimum,
                                               Maximum,
                                               RandomValue);

    return Status;
}

VOID
RandomReference(
    _In_opt_ PRANDOM Random
    )
{
    if (Random == NULL)
    {
        return;
    }

    Random->ReferenceCount += 1;
}

VOID
RandomDereference(
    _In_opt_ _Post_invalid_ PRANDOM Random
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Random == NULL)
    {
        return;
    }

    Random->ReferenceCount -= 1;

    if (Random->ReferenceCount == 0)
    {
        FreeRoutine = Random->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Random->Data);
        }

        IrisAlignedFree(Random);
    }
}