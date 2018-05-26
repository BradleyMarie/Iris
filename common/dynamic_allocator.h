/*++

Copyright (c) 2018 Brad Weinberger

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

struct _DYNAMIC_ALLOCATION;

_Struct_size_bytes_(size)
typedef struct _DYNAMIC_ALLOCATION {
    size_t size;
    struct _DYNAMIC_ALLOCATION *older;
    struct _DYNAMIC_ALLOCATION *newer;
} DYNAMIC_ALLOCATION, *PDYNAMIC_ALLOCATION;

typedef struct _DYNAMIC_MEMORY_ALLOCATOR {
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
        PDYNAMIC_ALLOCATION older = allocator->next_allocation->older;
        PDYNAMIC_ALLOCATION newer = allocator->next_allocation->newer;

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

        PDYNAMIC_ALLOCATION next_allocation = 
            (PDYNAMIC_ALLOCATION) allocation_header;

        if (allocator->next_allocation != next_allocation)
        {
            if (older != NULL)
            {
                older->newer = next_allocation;
            }

            if (newer != NULL)
            {
                newer->older = next_allocation;
            }

            next_allocation->size = allocation_size;
            next_allocation->older = older;
            next_allocation->newer = newer;
        }

        allocator->next_allocation = next_allocation->older;
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

        PDYNAMIC_ALLOCATION next_allocation = 
            (PDYNAMIC_ALLOCATION) allocation_header;

        PDYNAMIC_ALLOCATION last_allocation = allocator->last_allocation;

        if (last_allocation != NULL)
        {
            last_allocation->newer = next_allocation;
        }

        next_allocation->older = last_allocation;
        next_allocation->newer = NULL;
        next_allocation->size = allocation_size;

        allocator->last_allocation = next_allocation;
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

    allocator->next_allocation = allocator->last_allocation;
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

    PDYNAMIC_ALLOCATION next_allocation = allocator->last_allocation;
    allocator->last_allocation = NULL;

    while (next_allocation != NULL)
    {
        PDYNAMIC_ALLOCATION temp = next_allocation->older;
        free(next_allocation);
        next_allocation = temp;
    }
}

#endif // _COMMON_DYNAMIC_ALLOCATOR_