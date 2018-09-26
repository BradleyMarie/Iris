/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random.h

Abstract:

    Interface for random number generation.

--*/

#ifndef _IRIS_ADVANCED_RANDOM_
#define _IRIS_ADVANCED_RANDOM_

#include "common/free_routine.h"
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

typedef struct _RANDOM_VTABLE {
    PGENERATE_FLOAT_ROUTINE generate_float_routine;
    PGENERATE_INDEX_ROUTINE generate_index_routine;
    PFREE_ROUTINE free_routine;
} RANDOM_VTABLE, *PRANDOM_VTABLE;

typedef const RANDOM_VTABLE *PCRANDOM_VTABLE;

typedef struct _RANDOM RANDOM, *PRANDOM;
typedef const RANDOM *PCRANDOM;

//
// Functions
//

ISTATUS
RandomAllocate(
    _In_ PCRANDOM_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PRANDOM *rng
    );

ISTATUS
RandomGenerateFloat(
    _In_ PRANDOM rng,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    );

ISTATUS
RandomGenerateIndex(
    _In_ PRANDOM rng,
    _In_ size_t minimum,
    _In_ size_t maximum,
    _Out_range_(minimum, maximum) size_t *result
    );

void
RandomFree(
    _In_opt_ _Post_invalid_ PRANDOM rng
    );

#endif // _IRIS_ADVANCED_RANDOM_