/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    texture_coordinate_allocator.c

Abstract:

    The allocator for texture coordinates.

--*/

#include "iris_physx/texture_coordinate_allocator.h"
#include "iris_physx/texture_coordinate_allocator_internal.h"

//
// Functions
//

ISTATUS
TextureCoordinateAllocatorAllocate(
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _In_ size_t size,
    _In_ size_t alignment,
    _Out_ void **allocation
    )
{
    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (allocation == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (size == 0)
    {
        *allocation = NULL;
        return ISTATUS_SUCCESS;
    }

    if (alignment == 0 || (alignment & (alignment - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
    }

    if (size % alignment != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    void *data;
    bool success = DynamicMemoryAllocatorAllocate(&allocator->allocator,
                                                  NULL,
                                                  size,
                                                  alignment,
                                                  allocation,
                                                  0,
                                                  0,
                                                  &data);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    return ISTATUS_SUCCESS;
}