/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    dynamic_allocator.h

Abstract:

    An allocator for making quick allocations of a arbitrary size. This allocator 
    is neither intended to be efficient nor general purpose.

    The allocator is implemented using an array of pointers to blocks of memory.
    When a new allocation is requested, the allocator grabs the next pointer off
    the list and checks whether it is large enough to fulfill the allocation. 
    If so, it returns it. Otherwise, the chunk is resized and the new chunk is
    returned instead.

--*/

#ifndef _COMMON_DYNAMIC_ALLOCATOR_
#define _COMMON_DYNAMIC_ALLOCATOR_

#include <stdalign.h>

#include "common/alloc.h"

//
// Types
//

_Struct_size_bytes_(size)
typedef struct _DYNAMIC_ALLOCATION {
    size_t size;
    struct _DYNAMIC_ALLOCATION *prev;
    struct _DYNAMIC_ALLOCATION *next;
} DYNAMIC_ALLOCATION, *PDYNAMIC_ALLOCATION;

typedef struct _DYNAMIC_MEMORY_ALLOCATOR {
    PDYNAMIC_ALLOCATION first_allocation;
    PDYNAMIC_ALLOCATION last_allocation;
    PDYNAMIC_ALLOCATION next_allocation;
} DYNAMIC_MEMORY_ALLOCATOR, *PDYNAMIC_MEMORY_ALLOCATOR;

//
// Functions
//

static
inline
void
DynamicMemoryAllocatorInitialize(
    _Out_ PDYNAMIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    allocator->first_allocation = NULL;
    allocator->last_allocation = NULL;
    allocator->next_allocation = NULL;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
DynamicMemoryAllocatorAllocate(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR allocator,
    _Out_opt_ PDYNAMIC_ALLOCATION *allocation_handle,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && header_size % _Curr_ == 0) size_t header_alignment,
    _Outptr_result_bytebuffer_(header_size) void **header,
    _In_ size_t data_size,
    _When_(data_size != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _When_(data_size != 0, _Out_ _Deref_post_bytecap_(data_size)) _When_(data_size == 0, _Out_opt_ _When_(data != NULL, _Deref_post_null_)) void **data
    )
{
    assert(allocator != NULL);
    assert(header_size != 0);
    assert(header_alignment != 0);
    assert((header_alignment & (header_alignment - 1)) == 0);
    assert(header_size % header_alignment == 0);
    assert(header != NULL);
    assert(data_size == 0 || data_alignment != 0);
    assert(data_size == 0 || data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);

    if (allocator->next_allocation != NULL)
    {
        PDYNAMIC_ALLOCATION next_prev = allocator->next_allocation->prev;
        PDYNAMIC_ALLOCATION next_next = allocator->next_allocation->next;

        void *allocation_header;
        size_t allocation_size;
        bool ok = AlignedResizeWithTwoHeaders(allocator->next_allocation,
                                              allocator->next_allocation->size,
                                              sizeof(DYNAMIC_ALLOCATION),
                                              alignof(DYNAMIC_ALLOCATION),
                                              &allocation_header,
                                              header_size,
                                              header_alignment,
                                              header,
                                              data_size,
                                              data_alignment,
                                              data,
                                              &allocation_size);

        if (!ok)
        {
            return false;
        }

        PDYNAMIC_ALLOCATION result =
            (PDYNAMIC_ALLOCATION)allocation_header;

        if (allocator->next_allocation != result)
        {
            if (next_prev != NULL)
            {
                next_prev->next = result;
            }
            else
            {
                allocator->first_allocation = result;
            }

            if (next_next != NULL)
            {
                next_next->prev = result;
            }
            else
            {
                allocator->last_allocation = result;
            }

            result->size = allocation_size;
            result->next = next_next;
            result->prev = next_prev;
        }

        if (allocation_handle != NULL)
        {
            *allocation_handle = result;
        }

        allocator->next_allocation = result->next;
    }
    else
    {
        PDYNAMIC_ALLOCATION next_prev = allocator->last_allocation;

        void *allocation_header;
        size_t allocation_size;
        bool ok = AlignedAllocWithTwoHeaders(sizeof(DYNAMIC_ALLOCATION),
                                             alignof(DYNAMIC_ALLOCATION),
                                             &allocation_header,
                                             header_size,
                                             header_alignment,
                                             header,
                                             data_size,
                                             data_alignment,
                                             data,
                                             &allocation_size);

        if (!ok)
        {
            return false;
        }

        PDYNAMIC_ALLOCATION result =
            (PDYNAMIC_ALLOCATION)allocation_header;

        if (next_prev != NULL)
        {
            next_prev->next = result;
        }
        else
        {
            allocator->first_allocation = result;
        }

        allocator->last_allocation = result;

        if (allocation_handle != NULL)
        {
            *allocation_handle = result;
        }

        result->size = allocation_size;
        result->next = NULL;
        result->prev = next_prev;
    }

    return true;
}

static
inline
void
DynamicMemoryAllocatorFreeAll(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    allocator->next_allocation = allocator->first_allocation;
}

static
inline
void
DynamicMemoryAllocatorFreeAllExcept(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR allocator,
    _In_ PDYNAMIC_ALLOCATION allocation
    )
{
    assert(allocator != NULL);
    assert(allocation != NULL);

    if (allocator->first_allocation != allocation)
    {
        if (allocator->last_allocation != allocation)
        {
            allocation->next->prev = allocation->prev;
            allocation->prev->next = allocation->next;
        }
        else
        {
            allocator->last_allocation = allocation->prev;
            allocation->prev->next = NULL;
        }

        allocator->first_allocation->prev = allocation;

        allocation->next = allocator->first_allocation;
        allocation->prev = NULL;

        allocator->first_allocation = allocation;
    }

    allocator->next_allocation = allocation->next;
}

static
inline
void
DynamicMemoryAllocatorDestroy(
    _Post_invalid_ PDYNAMIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    allocator->next_allocation = NULL;

    PDYNAMIC_ALLOCATION next_allocation = allocator->first_allocation;

    while (next_allocation != NULL)
    {
        PDYNAMIC_ALLOCATION temp = next_allocation->next;
        free(next_allocation);
        next_allocation = temp;
    }
}

#endif // _COMMON_DYNAMIC_ALLOCATOR_