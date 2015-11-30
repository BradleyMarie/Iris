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
    PVOID Data;
};

struct _RANDOM_OWNER {
    PRANDOM Random;  
};

//
// Random Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
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

    ASSERT(Rng != NULL);

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

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
RandomFree(
    _In_ _Post_invalid_ PRANDOM Random
    )
{
    PFREE_ROUTINE FreeRoutine;

    ASSERT(Random != NULL);
    
    FreeRoutine = Random->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Random->Data);
    }

    IrisAlignedFree(Random);
}

///
// Random Public Functions
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

//
// RandomOwner Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomOwnerAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM_OWNER *Result
    )
{
    PRANDOM_OWNER RandomOwner;
    PRANDOM Random;
    ISTATUS Status;
    
    Status = RandomAllocate(RandomVTable,
                            Data,
                            DataSizeInBytes,
                            DataAlignment,
                            &Random);
                            
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    RandomOwner = (PRANDOM_OWNER) malloc(sizeof(RANDOM_OWNER));
    
    if (RandomOwner != NULL)
    {
        RandomFree(Random);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    RandomOwner->Random = Random;
    
    *Result = RandomOwner;
    
    return ISTATUS_SUCCESS;
}

_Ret_
IRISADVANCEDAPI
PRANDOM
RandomOwnerGetRandom(
    _In_ PRANDOM_OWNER RandomOwner
    )
{
    if (RandomOwner == NULL)
    {
        return NULL;
    }    
    
    return RandomOwner->Random;
}

IRISADVANCEDAPI
VOID
RandomOwnerFree(
    _In_opt_ _Post_invalid_ PRANDOM_OWNER RandomOwner
    )
{
    if (RandomOwner == NULL)
    {
        return;
    }
    
    RandomFree(RandomOwner->Random);
    free(RandomOwner);
}