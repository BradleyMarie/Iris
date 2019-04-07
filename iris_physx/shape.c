/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    shape.c

Abstract:

    Definitions for the shape interface.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/shape.h"
#include "iris_physx/shape_internal.h"

//
// Functions
//

ISTATUS
ShapeAllocate(
    _In_ PCSHAPE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSHAPE *shape
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

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SHAPE),
                                          alignof(SHAPE),
                                          (void **)shape,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*shape)->vtable = vtable;
    (*shape)->data = data_allocation;
    (*shape)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS 
ShapeComputeBounds(
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (world_bounds == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = shape->vtable->compute_bounds_routine(shape->data,
                                                           model_to_world,
                                                           world_bounds);

    return status;
}

void
ShapeRetain(
    _In_opt_ PSHAPE shape
    )
{
    if (shape == NULL)
    {
        return;
    }

    atomic_fetch_add(&shape->reference_count, 1);
}

void
ShapeRelease(
    _In_opt_ _Post_invalid_ PSHAPE shape
    )
{
    if (shape == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&shape->reference_count, 1) == 1)
    {
        if (shape->vtable->free_routine != NULL)
        {
            shape->vtable->free_routine(shape->data);
        }
    
        free(shape);
    }
}