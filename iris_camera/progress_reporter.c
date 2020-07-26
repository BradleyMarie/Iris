/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    progress_reporter.c

Abstract:

    Reports the progress of rendering as it occurs.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/progress_reporter_internal.h"
#include "iris_camera/progress_reporter.h"

//
// Functions
//

ISTATUS
ProgressReporterAllocate(
    _In_ PCPROGRESS_REPORTER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PPROGRESS_REPORTER *progress_reporter
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

    if (progress_reporter == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(PROGRESS_REPORTER),
                                          alignof(PROGRESS_REPORTER),
                                          (void **)progress_reporter,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*progress_reporter)->vtable = vtable;
    (*progress_reporter)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
ProgressReporterFree(
    _In_opt_ _Post_invalid_ PPROGRESS_REPORTER progress_reporter
    )
{
    if (progress_reporter == NULL)
    {
        return;
    }

    if (progress_reporter->vtable->free_routine != NULL)
    {
        progress_reporter->vtable->free_routine(progress_reporter->data);
    }

    free(progress_reporter);
}