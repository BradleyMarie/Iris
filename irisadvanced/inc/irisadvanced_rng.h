/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_random.h

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#ifndef _RANDOM_IRIS_ADVANCED_
#define _RANDOM_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef
ISTATUS
(*PGENERATE_FLOAT_ROUTINE)(
    _In_ PVOID Context,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

typedef
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM *Rng
    );

IRISADVANCEDAPI
ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

IRISADVANCEDAPI
ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    );

IRISADVANCEDAPI
VOID
RandomReference(
    _In_opt_ PRANDOM Random
    );

IRISADVANCEDAPI
VOID
RandomDereference(
    _In_opt_ _Post_invalid_ PRANDOM Random
    );

#endif // _RANDOM_IRIS_ADVANCED_