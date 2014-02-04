/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_random.h

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#ifndef _RANDOM_IRIS_SHADING_MODEL_
#define _RANDOM_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
FLOAT
(*PGENERATE_FLOAT_ROUTINE)(
    _In_ PVOID Context,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    );

typedef
SIZE_T
(*PGENERATE_INDEX_ROUTINE)(
    _In_ PVOID Context,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    );

typedef struct _RANDOM_VTABLE {
    PGENERATE_FLOAT_ROUTINE GenerateFloatRoutine;
    PGENERATE_INDEX_ROUTINE GenerateIndexRoutine;
    PFREE_ROUTINE FreeRoutine;
} RANDOM_VTABLE, *PRANDOM_VTABLE;

typedef CONST RANDOM_VTABLE *PCRANDOM_VTABLE;

typedef struct _RANDOM {
    PCRANDOM_VTABLE RandomVTable;
} RANDOM, *PRANDOM;

typedef CONST RANDOM *PCRANDOM;

//
// Functions
//

SFORCEINLINE
FLOAT
RandomGenerateFloat(
    _In_ PRANDOM Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    )
{
    FLOAT Result;

    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Minimum));
    ASSERT(IsFiniteFloat(Minimum));
    ASSERT(IsNormalFloat(Maximum));
    ASSERT(IsFiniteFloat(Minimum));
    ASSERT(Minimum < Maximum);

    Result = Rng->RandomVTable->GenerateFloatRoutine(Rng, Minimum, Maximum);

    ASSERT(IsNormalFloat(Result));
    ASSERT(IsFiniteFloat(Result));
    ASSERT(Minimum <= Result);
    ASSERT(Result <= Maximum);

    return Result;
}

SFORCEINLINE
SIZE_T
RandomGenerateIndex(
    _In_ PRANDOM Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    )
{
    SIZE_T Result;

    ASSERT(Rng != NULL);
    ASSERT(Minimum < Maximum);

    Result = Rng->RandomVTable->GenerateIndexRoutine(Rng, Minimum, Maximum);

    ASSERT(Minimum <= Result);
    ASSERT(Result <= Maximum);

    return Result;
}

SFORCEINLINE
VOID
RandomFree(
    _Pre_maybenull_ _Post_invalid_ PRANDOM Rng
    )
{
    if (Rng == NULL)
    {
        return;
    }

    Rng->RandomVTable->FreeRoutine(Rng);
}

#endif // _RANDOM_IRIS_SHADING_MODEL_