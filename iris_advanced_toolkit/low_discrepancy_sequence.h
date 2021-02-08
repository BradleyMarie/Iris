
/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    low_discrepancy_sequence.h

Abstract:

    Interface representing a low discrepancy sequence returning values between
    0.0 and 1.0.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_
#define _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_

#include "iris_advanced_toolkit/low_discrepancy_sequence_vtable.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LowDiscrepancySequenceAllocate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *sequence
    );

ISTATUS
LowDiscrepancySequencePermute(
    _Inout_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ PRANDOM rng
    );

ISTATUS
LowDiscrepancySequenceComputeIndex(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _In_ uint32_t sample,
    _In_ uint32_t num_samples,
    _Out_ uint64_t *index
    );

ISTATUS
LowDiscrepancySequenceStart(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _In_ uint64_t index
    );

ISTATUS
LowDiscrepancySequenceNextFloat(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ float_t *value
    );

ISTATUS
LowDiscrepancySequenceNextDouble(
    _In_ PLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ double_t *value
    );

ISTATUS
LowDiscrepancySequenceDuplicate(
    _In_ PCLOW_DISCREPANCY_SEQUENCE sequence,
    _Out_ PLOW_DISCREPANCY_SEQUENCE *duplicate
    );

void
LowDiscrepancySequenceFree(
    _In_opt_ _Post_invalid_ PLOW_DISCREPANCY_SEQUENCE sequence
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_ADVANCED_TOOLKIT_LOW_DISCREPANCY_SEQUENCE_