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
    RANDOM_REFERENCE RandomReference;
    SIZE_T ReferenceCount;
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

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    RandomReferenceInitialize(RandomVTable,
                              DataAllocation,
                              &AllocatedRng->RandomReference);

    AllocatedRng->ReferenceCount = 1;

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}

_Ret_
PRANDOM_REFERENCE
RandomGetRandomReference(
    _In_ PRANDOM Rng
    )
{
    if (Rng == NULL)
    {
        return NULL;
    }    
    
    return &Rng->RandomReference;
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
    
    //
    // &Rng->RandomReference should be safe to do even if
    // Rng == NULL.
    //
    
    Status = RandomReferenceGenerateFloat(&Rng->RandomReference,
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
    
    //
    // &Rng->RandomReference should be safe to do even if
    // Rng == NULL.
    //
    
    Status = RandomReferenceGenerateIndex(&Rng->RandomReference,
                                          Minimum,
                                          Maximum,
                                          RandomValue);

    return Status;
}

VOID
RandomRetain(
    _In_opt_ PRANDOM Rng
    )
{
    if (Rng == NULL)
    {
        return;
    }

    Rng->ReferenceCount += 1;
}

VOID
RandomRelease(
    _In_opt_ _Post_invalid_ PRANDOM Rng
    )
{
    if (Rng == NULL)
    {
        return;
    }

    Rng->ReferenceCount -= 1;

    if (Rng->ReferenceCount == 0)
    {
        RandomReferenceDestroy(&Rng->RandomReference);
        IrisAlignedFree(Rng);
    }
}