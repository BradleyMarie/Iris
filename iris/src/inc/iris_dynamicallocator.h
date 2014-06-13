/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_dynamicallocator.h

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

#ifndef _IRIS_DYNAMIC_MEMORY_ALLOCATOR_INTERNAL_
#define _IRIS_DYNAMIC_MEMORY_ALLOCATOR_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _IRIS_DYNAMIC_MEMORY_ALLOCATION;

_Struct_size_bytes_(SizeInBytes)
typedef struct _IRIS_DYNAMIC_MEMORY_ALLOCATION {
    SIZE_T SizeInBytes;
    struct _IRIS_DYNAMIC_MEMORY_ALLOCATION *OlderAllocation;
    struct _IRIS_DYNAMIC_MEMORY_ALLOCATION *NewerAllocation;
} IRIS_DYNAMIC_MEMORY_ALLOCATION, *PIRIS_DYNAMIC_MEMORY_ALLOCATION;

typedef struct _IRIS_DYNAMIC_MEMORY_ALLOCATOR {
    PIRIS_DYNAMIC_MEMORY_ALLOCATION AllocationListTail;
    PIRIS_DYNAMIC_MEMORY_ALLOCATION NextAllocation;
} IRIS_DYNAMIC_MEMORY_ALLOCATOR, *PIRIS_DYNAMIC_MEMORY_ALLOCATOR;

//
// Functions
//

SFORCEINLINE
VOID
IrisDynamicMemoryAllocatorInitialize(
    _Out_ PIRIS_DYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->AllocationListTail = NULL;
    Allocator->NextAllocation = NULL;
}

_When_(SizeInBytes != 0, _Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(SizeInBytes))
SFORCEINLINE
PVOID
IrisDynamicMemoryAllocatorAllocate(
    _Inout_ PIRIS_DYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_ SIZE_T SizeInBytes
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATION NewerAllocation;
    PIRIS_DYNAMIC_MEMORY_ALLOCATION OlderAllocation;
    PIRIS_DYNAMIC_MEMORY_ALLOCATION IrisAllocation;
    SIZE_T AllocationSize;
    PVOID Allocation;
    ISTATUS Status;
    PVOID Resized;

    ASSERT(Allocator != NULL);
    ASSERT(SizeInBytes != 0);

    Status = CheckedAddSizeT(SizeInBytes, 
                             sizeof(IRIS_DYNAMIC_MEMORY_ALLOCATION),
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (Allocator->NextAllocation != NULL)
    {
        IrisAllocation = Allocator->NextAllocation;

        if (IrisAllocation->SizeInBytes < SizeInBytes)
        {
            Resized = realloc(IrisAllocation, AllocationSize);

            if (Resized == NULL)
            {
                return NULL;
            }

            IrisAllocation = (PIRIS_DYNAMIC_MEMORY_ALLOCATION) Resized;

            OlderAllocation = IrisAllocation->OlderAllocation;
            NewerAllocation = IrisAllocation->NewerAllocation;

            if (OlderAllocation != NULL)
            {
                OlderAllocation->NewerAllocation = IrisAllocation;
            }

            if (NewerAllocation != NULL)
            {
                NewerAllocation->OlderAllocation = IrisAllocation;
            }

            IrisAllocation->SizeInBytes = SizeInBytes;
        }

        Allocator->NextAllocation = IrisAllocation->OlderAllocation;
    }
    else
    {        
        Allocation = malloc(AllocationSize);

        if (Allocation == NULL)
        {
            return NULL;
        }

        IrisAllocation = (PIRIS_DYNAMIC_MEMORY_ALLOCATION) Allocation;

        OlderAllocation = Allocator->AllocationListTail;

        if (OlderAllocation != NULL)
        {
            OlderAllocation->NewerAllocation = IrisAllocation;
        }

        IrisAllocation->OlderAllocation = OlderAllocation;
        IrisAllocation->NewerAllocation = NULL;
        IrisAllocation->SizeInBytes = SizeInBytes;

        Allocator->AllocationListTail = IrisAllocation;
    }

    return (PCHAR) IrisAllocation + sizeof(IRIS_DYNAMIC_MEMORY_ALLOCATION);
}

SFORCEINLINE
VOID
IrisDynamicMemoryAllocatorFreeAll(
    _Inout_ PIRIS_DYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = Allocator->AllocationListTail;
}

SFORCEINLINE
VOID
IrisDynamicMemoryAllocatorDestroy(
    _Inout_ PIRIS_DYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATION NextAllocation;
    PIRIS_DYNAMIC_MEMORY_ALLOCATION Temp;

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

#endif _IRIS_DYNAMIC_MEMORY_ALLOCATOR_INTERNAL_