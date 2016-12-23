/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_random.h

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#ifndef _RANDOM_IRIS_ADVANCED_RANDOM_
#define _RANDOM_IRIS_ADVANCED_RANDOM_

#include <irisadvanced.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PGENERATE_FLOAT_ROUTINE)(
    _In_ PVOID Context,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PGENERATE_INDEX_ROUTINE)(
    _In_ PVOID Context,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    );

typedef struct _RANDOM_VTABLE {
    PGENERATE_FLOAT_ROUTINE GenerateFloatRoutine;
    PGENERATE_INDEX_ROUTINE GenerateIndexRoutine;
    PFREE_ROUTINE FreeRoutine;
} RANDOM_VTABLE, *PRANDOM_VTABLE;

typedef CONST RANDOM_VTABLE *PCRANDOM_VTABLE;

typedef struct _RANDOM RANDOM, *PRANDOM;
typedef CONST RANDOM *PCRANDOM;

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    );

#endif // _RANDOM_IRIS_ADVANCED_RANDOM_