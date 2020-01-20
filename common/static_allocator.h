/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    static_allocator.h

Abstract:

    An allocator for making quick allocations of a fixed size. This allocator 
    is neither intended to be efficient nor general purpose.

    The allocator is implemented using an array of pointers to regions of the
    same size. When a new region is allocated, the next available block is
    returned if one is present. Otherwise, a new region is allocated and 
    appended to the list.

--*/

#ifndef _COMMON_STATIC_ALLOCATOR_
#define _COMMON_STATIC_ALLOCATOR_

#include "common/pointer_list.h"

//
// Types
//

typedef struct _STATIC_MEMORY_ALLOCATOR {
    POINTER_LIST chunks;
    size_t allocation_size;
    size_t current_size;
} STATIC_MEMORY_ALLOCATOR, *PSTATIC_MEMORY_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
StaticMemoryAllocatorInitialize(
    _Out_ PSTATIC_MEMORY_ALLOCATOR allocator,
    _In_ size_t allocation_size
    )
{
    assert(allocator != NULL);
    assert(allocation_size != 0);

    bool success = PointerListInitialize(&allocator->chunks);

    if (!success)
    {
        return false;
    }

    allocator->allocation_size = allocation_size;
    allocator->current_size = 0;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
StaticMemoryAllocatorAllocate(
    _Inout_ PSTATIC_MEMORY_ALLOCATOR allocator,
    _Outptr_result_bytebuffer_(_Inexpressible_(allocator->allocator_size)) void **allocation
    )
{
    assert(allocator != NULL);

    size_t num_chunks = PointerListGetSize(&allocator->chunks);

    void *tmp_allocation;
    if (allocator->current_size == num_chunks)
    {
        bool success = PointerListPrepareToAddPointers(&allocator->chunks, 1);

        if (!success)
        {
            return false;
        }

        tmp_allocation = malloc(allocator->allocation_size);

        if (tmp_allocation == NULL)
        {
            return false;
        }

         PointerListAddPointer(&allocator->chunks, tmp_allocation);
    }
    else
    {
        tmp_allocation = PointerListRetrieveAtIndex(&allocator->chunks,
                                                    allocator->current_size);
    }

    *allocation = tmp_allocation;
    allocator->current_size++;

    return true;
}

static
inline
void
StaticMemoryAllocatorFreeAll(
    _Inout_ PSTATIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    allocator->current_size = 0;
}

static
inline
void
StaticMemoryAllocatorDestroy(
    _Inout_ PSTATIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    size_t num_chunks = PointerListGetSize(&allocator->chunks);

    for (size_t i = 0; i < num_chunks; i++)
    {
        void *allocation = PointerListRetrieveAtIndex(&allocator->chunks, i);
        free(allocation);
    }

    PointerListDestroy(&allocator->chunks);
    allocator->allocation_size = 0;
    allocator->current_size = 0;
}

#endif // _COMMON_STATIC_ALLOCATOR_