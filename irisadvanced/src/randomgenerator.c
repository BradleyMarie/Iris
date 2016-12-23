/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    randomgenerator.c

Abstract:

    This file contains the definitions for the RANDOM_GENERATOR type.

--*/

#include <irisadvancedp.h>

//
// Types
//

struct _RANDOM_GENERATOR {
    PCRANDOM_GENERATOR_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomGeneratorAllocate(
    _In_ PCRANDOM_GENERATOR_VTABLE RandomGeneratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM_GENERATOR *RngGenerator
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PRANDOM_GENERATOR AllocatedGenerator;

    if (RandomGeneratorVTable == NULL)
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

    if (RngGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(RANDOM_GENERATOR),
                                                      _Alignof(RANDOM_GENERATOR),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedGenerator = (PRANDOM_GENERATOR) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedGenerator->VTable = RandomGeneratorVTable;
    AllocatedGenerator->Data = DataAllocation;

    *RngGenerator = AllocatedGenerator;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomGeneratorGenerate(
    _In_ PCRANDOM_GENERATOR RngGenerator,
    _In_ PRANDOM_GENERATOR_RANDOM_LIFETIME_SCOPE_ROUTINE Callback,
    _Inout_opt_ PVOID CallbackContext
    )
{
    PRANDOM_ALLOCATOR RandomAllocator;
    PRANDOM Rng;
    ISTATUS Status;
    
    if (RngGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Status = RandomAllocatorAllocate(&RandomAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RngGenerator->VTable->GenerateRandomRoutine(RngGenerator->Data,
                                                         RandomAllocator,
                                                         &Rng);

    if (Status != ISTATUS_SUCCESS)
    {
        RandomAllocatorFree(RandomAllocator);
        return Status;
    }

    Callback(CallbackContext, Rng);
    RandomAllocatorFree(RandomAllocator);

    return ISTATUS_SUCCESS;
}

VOID
RandomGeneratorFree(
    _In_opt_ _Post_invalid_ PRANDOM_GENERATOR RngGenerator
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    if (RngGenerator == NULL)
    {
        return;
    }
    
    FreeRoutine = RngGenerator->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(RngGenerator->Data);
    }

    IrisAlignedFree(RngGenerator);
}