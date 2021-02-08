/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    low_discrepancy_sequence_vtable.h

Abstract:

    The vtable for a low discrepancy sequence.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_VTABLE_
#define _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_VTABLE_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _LOW_DISCREPANCY_SEQUENCE LOW_DISCREPANCY_SEQUENCE, *PLOW_DISCREPANCY_SEQUENCE;
typedef const LOW_DISCREPANCY_SEQUENCE *PCLOW_DISCREPANCY_SEQUENCE;

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_PERMUTE_ROUTINE)(
    _Inout_ void *context,
    _In_ PRANDOM rng
    );

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_COMPUTE_INDEX_ROUTINE)(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _In_ uint32_t sample,
    _In_ uint32_t num_samples,
    _Out_ uint64_t *index
    );

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_START_ROUTINE)(
    _In_ void *context,
    _In_ uint64_t index
    );

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_NEXT_FLOAT_ROUTINE)(
    _In_ void *context,
    _Out_ float_t *value
    );

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_NEXT_DOUBLE_ROUTINE)(
    _In_ void *context,
    _Out_ double_t *value
    );

typedef
ISTATUS
(*PLOW_DISCREPANCY_SEQUENCE_DUPLICATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    );

typedef struct _LOW_DISCREPANCY_SEQUENCE_VTABLE {
    PLOW_DISCREPANCY_SEQUENCE_PERMUTE_ROUTINE permute_routine;
    PLOW_DISCREPANCY_SEQUENCE_COMPUTE_INDEX_ROUTINE compute_index_routine;
    PLOW_DISCREPANCY_SEQUENCE_START_ROUTINE start_routine;
    PLOW_DISCREPANCY_SEQUENCE_NEXT_FLOAT_ROUTINE next_float_routine;
    PLOW_DISCREPANCY_SEQUENCE_NEXT_DOUBLE_ROUTINE next_double_routine;
    PLOW_DISCREPANCY_SEQUENCE_DUPLICATE_ROUTINE duplicate_routine;
    PFREE_ROUTINE free_routine;
} LOW_DISCREPANCY_SEQUENCE_VTABLE, *PLOW_DISCREPANCY_SEQUENCE_VTABLE;

typedef const LOW_DISCREPANCY_SEQUENCE_VTABLE *PCLOW_DISCREPANCY_SEQUENCE_VTABLE;

#endif // _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_VTABLE_