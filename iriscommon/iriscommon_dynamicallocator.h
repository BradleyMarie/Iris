/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iriscommon_dynamicallocator.h

Abstract:

    This module implements the internal dynamic memory allocator used by Iris. 
    This allocator is not intended to be efficient nor is it intend to be a 
    general purpose allocator. It's just intended to be fast and provide 
    allocations of a dynamic size.

    The allocator works by simply creating a linked list of free regions of
    memory. When a region of memory is allocated, it just returns the first
    element off the list, expanding the region if necessary. Elements can only
    be freed in bulk.

--*/

#ifndef _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_
#define _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_

#include <iris.h>

//
// Types
//

struct _DYNAMIC_MEMORY_ALLOCATION;

_Struct_size_bytes_(SizeInBytes)
typedef struct _DYNAMIC_MEMORY_ALLOCATION {
    SIZE_T SizeInBytes;
    struct _DYNAMIC_MEMORY_ALLOCATION *OlderAllocation;
    struct _DYNAMIC_MEMORY_ALLOCATION *NewerAllocation;
} DYNAMIC_MEMORY_ALLOCATION, *PDYNAMIC_MEMORY_ALLOCATION;

typedef struct _DYNAMIC_MEMORY_ALLOCATOR {
    PDYNAMIC_MEMORY_ALLOCATION AllocationListTail;
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
} DYNAMIC_MEMORY_ALLOCATOR, *PDYNAMIC_MEMORY_ALLOCATOR;

//
// Functions
//

SFORCEINLINE
VOID
DynamicMemoryAllocatorInitialize(
    _Out_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->AllocationListTail = NULL;
    Allocator->NextAllocation = NULL;
}

_Check_return_ 
_Ret_maybenull_ 
_Post_writable_byte_size_(SizeInBytes)
SFORCEINLINE
PVOID
DynamicMemoryAllocatorAllocate(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_range_(1, SIZE_T_MAX) SIZE_T SizeInBytes
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NewerAllocation;
    PDYNAMIC_MEMORY_ALLOCATION OlderAllocation;
    PDYNAMIC_MEMORY_ALLOCATION DynamicAllocation;
    SIZE_T AllocationSize;
    PVOID Allocation;
    ISTATUS Status;
    PVOID Resized;

    ASSERT(Allocator != NULL);
    ASSERT(SizeInBytes != 0);

    Status = CheckedAddSizeT(SizeInBytes, 
                             sizeof(DYNAMIC_MEMORY_ALLOCATION),
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (Allocator->NextAllocation != NULL)
    {
        DynamicAllocation = Allocator->NextAllocation;

        if (DynamicAllocation->SizeInBytes < SizeInBytes)
        {
            Resized = realloc(DynamicAllocation, AllocationSize);

            if (Resized == NULL)
            {
                return NULL;
            }

            DynamicAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Resized;

            OlderAllocation = DynamicAllocation->OlderAllocation;
            NewerAllocation = DynamicAllocation->NewerAllocation;

            if (OlderAllocation != NULL)
            {
                OlderAllocation->NewerAllocation = DynamicAllocation;
            }

            if (NewerAllocation != NULL)
            {
                NewerAllocation->OlderAllocation = DynamicAllocation;
            }

            DynamicAllocation->SizeInBytes = SizeInBytes;
        }

        Allocator->NextAllocation = DynamicAllocation->OlderAllocation;
    }
    else
    {        
        Allocation = malloc(AllocationSize);

        if (Allocation == NULL)
        {
            return NULL;
        }

        DynamicAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Allocation;

        OlderAllocation = Allocator->AllocationListTail;

        if (OlderAllocation != NULL)
        {
            OlderAllocation->NewerAllocation = DynamicAllocation;
        }

        DynamicAllocation->OlderAllocation = OlderAllocation;
        DynamicAllocation->NewerAllocation = NULL;
        DynamicAllocation->SizeInBytes = SizeInBytes;

        Allocator->AllocationListTail = DynamicAllocation;
    }

    return (PVOID) ((PUINT8) DynamicAllocation + sizeof(DYNAMIC_MEMORY_ALLOCATION));
}

SFORCEINLINE
VOID
DynamicMemoryAllocatorFreeAll(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = Allocator->AllocationListTail;
}

SFORCEINLINE
VOID
DynamicMemoryAllocatorDestroy(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
    PDYNAMIC_MEMORY_ALLOCATION Temp;

    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = NULL;

    NextAllocation = Allocator->AllocationListTail;
    Allocator->AllocationListTail = NULL;

    while (NextAllocation != NULL)
    {
        Temp = NextAllocation->OlderAllocation;

        free(NextAllocation);

        NextAllocation = Temp;
    }
}

#endif // _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_