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
    PDYNAMIC_ALLOCATION next_allocation;
    DYNAMIC_ALLOCATION head;
    DYNAMIC_ALLOCATION tail;
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

    allocator->next_allocation = &allocator->tail;
    allocator->head.size = 0;
    allocator->head.prev = NULL;
    allocator->head.next = &allocator->tail;
    allocator->tail.size = 0;
    allocator->tail.prev = &allocator->head;
    allocator->tail.next = NULL;
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

    if (allocator->next_allocation != &allocator->tail)
    {
        PDYNAMIC_ALLOCATION prev = allocator->next_allocation->prev;
        PDYNAMIC_ALLOCATION next = allocator->next_allocation->next;

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
            prev->next = result;
            next->prev = result;
            result->size = allocation_size;
            result->prev = prev;
            result->next = next;
        }

        if (allocation_handle != NULL)
        {
            *allocation_handle = result;
        }

        allocator->next_allocation = result->next;
    }
    else
    {
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

        PDYNAMIC_ALLOCATION prev = allocator->tail.prev;
        PDYNAMIC_ALLOCATION next = &allocator->tail;
        prev->next = result;
        next->prev = result;
        result->size = allocation_size;
        result->next = next;
        result->prev = prev;

        if (allocation_handle != NULL)
        {
            *allocation_handle = result;
        }
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

    allocator->next_allocation = allocator->head.next;
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

    allocation->prev->next = allocation->next;
    allocation->next->prev = allocation->prev;

    PDYNAMIC_ALLOCATION next = allocator->head.next;
    allocation->next = next;
    allocation->prev = &allocator->head;

    next->prev = allocation;
    allocator->head.next = allocation;

    allocator->next_allocation = next;
}

static
inline
void
DynamicMemoryAllocatorDestroy(
    _Post_invalid_ PDYNAMIC_MEMORY_ALLOCATOR allocator
    )
{
    assert(allocator != NULL);

    PDYNAMIC_ALLOCATION next_allocation = allocator->head.next;
    while (next_allocation != &allocator->tail)
    {
        PDYNAMIC_ALLOCATION temp = next_allocation->next;
        free(next_allocation);
        next_allocation = temp;
    }

    allocator->next_allocation = NULL;
    allocator->head.next = &allocator->tail;
    allocator->tail.prev = &allocator->head;
}

#endif // _COMMON_DYNAMIC_ALLOCATOR_