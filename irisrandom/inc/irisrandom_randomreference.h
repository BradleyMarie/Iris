/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisrandom_randomreference.h

Abstract:

    This file contains the definitions for the RANDOM_REFERENCE type.

--*/

#include <irisrandom.h>

#ifndef _RANDOM_REFERENCE_IRIS_RANDOM_
#define _RANDOM_REFERENCE_IRIS_RANDOM_

//
// Types
//

typedef struct _RANDOM_REFERENCE RANDOM_REFERENCE, *PRANDOM_REFERENCE;
typedef CONST RANDOM_REFERENCE *PCRANDOM_REFERENCE;

//
// Functions
//

#ifndef _DISABLE_IRIS_RANDOM_RANDOM_REFERENCE_EXPORTS_

_Success_(return == ISTATUS_SUCCESS)
IRISRANDOMAPI
ISTATUS
RandomReferenceGenerateFloat(
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    );

_Success_(return == ISTATUS_SUCCESS)
IRISRANDOMAPI
ISTATUS
RandomReferenceGenerateIndex(
    _In_ PRANDOM_REFERENCE Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    );

#endif // _DISABLE_IRIS_RANDOM_RANDOM_REFERENCE_EXPORTS_

#endif // _RANDOM_REFERENCE_IRIS_RANDOM_