/*++

Copyright (c) 2016 Brad Weinberger

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
    PVOID Data;
};

//
// Private Functions
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

    ASSERT(RandomVTable != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(Rng != NULL);

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

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedRng->VTable = RandomVTable;
    AllocatedRng->Data = DataAllocation;

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}

VOID
RandomFree(
    _In_opt_ _Post_invalid_ PRANDOM Rng
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    ASSERT(Rng != NULL);
    
    FreeRoutine = Rng->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Rng->Data);
    }

    IrisAlignedFree(Rng);
}

//
// Public Functions
//

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

    if (IsFiniteFloat(Minimum) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Maximum) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
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