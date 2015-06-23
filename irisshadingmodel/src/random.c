/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_random.h

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _RANDOM {
    PCRANDOM_VTABLE RandomVTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PRANDOM
RandomAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PRANDOM Random;

    if (RandomVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(RANDOM),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Random = (PRANDOM) HeaderAllocation;

    Random->RandomVTable = RandomVTable;
    Random->Data = DataAllocation;
    Random->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Random;
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

    if (Rng == NULL ||
        IsNormalFloat(Minimum) == FALSE ||
        IsFiniteFloat(Minimum) == FALSE ||
        IsNormalFloat(Maximum) == FALSE ||
        IsFiniteFloat(Maximum) == FALSE ||
        Minimum > Maximum ||
        RandomValue == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = Rng->RandomVTable->GenerateFloatRoutine(Rng->Data,
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

    if (Rng == NULL ||
        Minimum > Maximum ||
        RandomValue == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = Rng->RandomVTable->GenerateIndexRoutine(Rng->Data,
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
        FreeRoutine = Random->RandomVTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Random->Data);
        }

        IrisAlignedFree(Random);
    }
}