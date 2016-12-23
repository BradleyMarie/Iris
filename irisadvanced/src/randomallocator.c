/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    randomallocator.c

Abstract:

    This file contains the definitions for the RANDOM_ALLOCATOR and 
    RANDOM_ALLOCATOR_REFERENCE types.

--*/

#include <irisadvancedp.h>
#include <iriscommon_pointerlist.h>

//
// Types
//

struct _RANDOM_ALLOCATOR {
    POINTER_LIST AllocatedRngs;
};

//
// Private Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomAllocatorAllocate(
    _Out_ PRANDOM_ALLOCATOR *RandomAllocator
    )
{
    PRANDOM_ALLOCATOR AllocatedRandomAllocator;
    ISTATUS Status;

    if (RandomAllocator != NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    AllocatedRandomAllocator = (PRANDOM_ALLOCATOR) malloc(sizeof(RANDOM_ALLOCATOR));

    if (AllocatedRandomAllocator == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = PointerListInitialize(&AllocatedRandomAllocator->AllocatedRngs);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(AllocatedRandomAllocator);
        return Status;
    }

    return ISTATUS_SUCCESS;
}

VOID
RandomAllocatorClear(
    _Inout_ PRANDOM_ALLOCATOR RandomAllocator
    )
{
    SIZE_T Index;
    SIZE_T NumberOfRngs;
    PRANDOM Rng;

    if (RandomAllocator == NULL)
    {
        return;
    }

    NumberOfRngs = PointerListGetSize(&RandomAllocator->AllocatedRngs);

    for (Index = 0; Index < NumberOfRngs; Index += 1)
    {
        Rng = (PRANDOM) PointerListRetrieveAtIndex(&RandomAllocator->AllocatedRngs, 
                                                   Index);

        RandomFree(Rng);
    }

    PointerListClear(&RandomAllocator->AllocatedRngs);
}

VOID
RandomAllocatorFree(
    _In_opt_ _Post_invalid_ PRANDOM_ALLOCATOR RandomAllocator
    )
{
    if (RandomAllocator == NULL)
    {
        return;
    }

    RandomAllocatorClear(RandomAllocator);
    PointerListDestroy(&RandomAllocator->AllocatedRngs);
    free(RandomAllocator);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomAllocatorAllocateRandom(
    _Inout_ PRANDOM_ALLOCATOR RandomAllocator,
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM *Rng
    )
{
    PRANDOM AllocatedRng;
    ISTATUS Status;

    if (RandomAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RandomVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = PointerListPrepareToAddPointers(&RandomAllocator->AllocatedRngs, 1);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    Status = RandomAllocate(RandomVTable,
                            Data,
                            DataSizeInBytes,
                            DataAlignment,
                            &AllocatedRng);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    PointerListAddPointer(&RandomAllocator->AllocatedRngs, AllocatedRng);

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}