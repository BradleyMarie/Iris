/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random.h

Abstract:

    Interface for random number generation.

--*/

#ifndef _IRIS_ADVANCED_RANDOM_
#define _IRIS_ADVANCED_RANDOM_

#include "iris/iris.h"

//
// Types
//

typedef
ISTATUS
(*PGENERATE_FLOAT_ROUTINE)(
    _In_ void *context,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    );

typedef
ISTATUS
(*PGENERATE_INDEX_ROUTINE)(
    _In_ void *context,
    _In_ size_t minimum,
    _In_ size_t maximum,
    _Out_range_(minimum, maximum) size_t *result
    );

typedef struct _RANDOM RANDOM, *PRANDOM;
typedef const RANDOM *PCRANDOM;

typedef
ISTATUS
(*PRANDOM_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PRANDOM rng
    );

//
// Functions
//

//IRISADVANCEDAPI
ISTATUS
RandomCreate(
    _In_ PGENERATE_FLOAT_ROUTINE generate_float_routine,
    _In_ PGENERATE_INDEX_ROUTINE generate_index_routine,
    _Inout_opt_ void *rng_context,
    _In_ PRANDOM_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );

//IRISADVANCEDAPI
ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM rng,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    );

//IRISADVANCEDAPI
ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM rng,
    _In_ size_t minimum,
    _In_ size_t maximum,
    _Out_range_(minimum, maximum) size_t *result
    );

#endif // _IRIS_ADVANCED_RANDOM_