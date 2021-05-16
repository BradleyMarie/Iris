/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    sample_tracer.c

Abstract:

    Interface for sample tracer.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/sample_tracer.h"
#include "iris_camera/sample_tracer_internal.h"

//
// Functions
//

ISTATUS
SampleTracerAllocate(
    _In_ PCSAMPLE_TRACER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SAMPLE_TRACER),
                                          alignof(SAMPLE_TRACER),
                                          (void **)sample_tracer,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*sample_tracer)->vtable = vtable;
    (*sample_tracer)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
SampleTracerFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER sample_tracer
    )
{
    if (sample_tracer == NULL)
    {
        return;
    }

    if (sample_tracer->vtable->free_routine)
    {
        sample_tracer->vtable->free_routine(sample_tracer->data);
    }

    free(sample_tracer);
}