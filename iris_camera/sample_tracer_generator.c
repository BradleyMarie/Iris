/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer_generator.c

Abstract:

    Interface for sample tracer generator.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/sample_tracer_generator.h"
#include "iris_camera/sample_tracer_generator_internal.h"

//
// Functions
//

ISTATUS
SampleTracerGeneratorAllocate(
    _In_ PCSAMPLE_TRACER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSAMPLE_TRACER_GENERATOR *sample_tracer_generator
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

    if (sample_tracer_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SAMPLE_TRACER_GENERATOR),
                                          alignof(SAMPLE_TRACER_GENERATOR),
                                          (void **)sample_tracer_generator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*sample_tracer_generator)->vtable = vtable;
    (*sample_tracer_generator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
SampleTracerGeneratorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_GENERATOR sample_tracer_generator
    )
{
    if (sample_tracer_generator == NULL)
    {
        return;
    }

    if (sample_tracer_generator->vtable->free_routine)
    {
        sample_tracer_generator->vtable->free_routine(
            sample_tracer_generator->data);
    }

    free(sample_tracer_generator);
}