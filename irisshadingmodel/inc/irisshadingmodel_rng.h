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
_Ret_maybenull_
IRISSHADINGMODELAPI
PRANDOM
RandomAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

IRISSHADINGMODELAPI
ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

IRISSHADINGMODELAPI
ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    );

IRISSHADINGMODELAPI
VOID
RandomReference(
    _In_opt_ PRANDOM Random
    );

IRISSHADINGMODELAPI
VOID
RandomDereference(
    _In_opt_ _Post_invalid_ PRANDOM Random
    );

#endif // _RANDOM_IRIS_SHADING_MODEL_