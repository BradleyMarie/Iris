/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_randomreference.h

Abstract:

    Internal definitions and declarations for randomreference type.

--*/

#ifndef _IRIS_ADVANCED_RANDOM_REFERENCE_INTERNAL_
#define _IRIS_ADVANCED_RANDOM_REFERENCE_INTERNAL_

#include <irisadvancedp.h>

//
// Macros
//

#ifdef _IRIS_ADVANCED_EXPORT_RANDOM_REFERENCE_ROUTINES_
#define RandomReferenceGenerateFloat(Rng,Minimum,Maximum,RandomValue) \
        StaticRandomReferenceGenerateFloat(Rng,Minimum,Maximum,RandomValue)

#define RandomReferenceGenerateIndex(Rng,Minimum,Maximum,RandomValue) \
        StaticRandomReferenceGenerateIndex(Rng,Minimum,Maximum,RandomValue)
#endif

//
// Types
//

struct _RANDOM_REFERENCE {
    PCRANDOM_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
RandomReferenceInitialize(
    _In_ PCRANDOM_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PRANDOM_REFERENCE RandomReference
    )
{
    ASSERT(VTable != NULL);
    ASSERT(RandomReference != NULL);
    
    RandomReference->VTable = VTable;
    RandomReference->Data = Data;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RandomReferenceGenerateFloat(
    _In_ PRANDOM_REFERENCE Rng,
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

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RandomReferenceGenerateIndex(
    _In_ PRANDOM_REFERENCE Rng,
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

SFORCEINLINE
VOID
RandomReferenceDestroy(
    _In_ _Post_invalid_ PRANDOM_REFERENCE RandomReference
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    ASSERT(RandomReference != NULL);
    
    FreeRoutine = RandomReference->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(RandomReference->Data);
    }
}

#ifdef _IRIS_ADVANCED_EXPORT_RANDOM_REFERENCE_ROUTINES_
#undef RandomReferenceGenerateFloat
#undef RandomReferenceGenerateIndex
#endif

#endif // _IRIS_ADVANCED_RANDOM_REFERENCE_INTERNAL_