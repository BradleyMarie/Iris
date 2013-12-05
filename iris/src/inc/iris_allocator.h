/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_allocator.h

Abstract:

    This module implements the internal memory allocator used by Iris. This
    allocator is not intended to be efficient nor is it intend to be a general
    purpose allocator. It's just intended to be fast.

    The allocator works by simply creating a linked list of free regions of
    memory. When a region of memory is allocated, it just returns the first
    element off the list, expanding the region if necessary. Elements can only
    be freed in bulk.

--*/

#ifndef _IRIS_MEMORY_ALLOCATOR_INTERNAL_
#define _IRIS_MEMORY_ALLOCATOR_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _IRIS_MEMORY_ALLOCATION;

_Struct_size_bytes_(SizeInBytes)
typedef struct _IRIS_MEMORY_ALLOCATION {
    SIZE_T SizeInBytes;
	struct _IRIS_MEMORY_ALLOCATION *OlderAllocation;
	struct _IRIS_MEMORY_ALLOCATION *NewerAllocation;
} IRIS_MEMORY_ALLOCATION, *PIRIS_MEMORY_ALLOCATION;

typedef struct _IRIS_MEMORY_ALLOCATOR {
    PIRIS_MEMORY_ALLOCATION AllocationListTail;
    PIRIS_MEMORY_ALLOCATION NextAllocation;
} IRIS_MEMORY_ALLOCATOR, *PIRIS_MEMORY_ALLOCATOR;

//
// Functions
//

SFORCEINLINE
VOID
IrisMemoryAllocatorInitialize(
    _Out_ PIRIS_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->AllocationListTail = NULL;
    Allocator->NextAllocation = NULL;
}

_Check_return_
_Ret_maybenull_
_Post_writable_byte_size_(Size)
SFORCEINLINE
PVOID
IrisMemoryAllocatorAllocate(
    _Inout_ PIRIS_MEMORY_ALLOCATOR Allocator,
	_In_ SIZE_T SizeInBytes
    )
{
    PIRIS_MEMORY_ALLOCATION IrisAllocation;
    PVOID ResizedAllocation;
	PVOID Allocation;

    ASSERT(Allocator != NULL);

	if (SizeInBytes == 0)
    {
        return NULL;
    }

    if (Allocator->NextAllocation != NULL)
    {
        IrisAllocation = Allocator->NextAllocation;

		if (IrisAllocation->SizeInBytes < SizeInBytes)
        {
			ResizedAllocation = realloc(IrisAllocation, SizeInBytes);

            if (ResizedAllocation == NULL)
            {
                return NULL;
            }

            IrisAllocation = (PIRIS_MEMORY_ALLOCATION) ResizedAllocation;

            IrisAllocation->OlderAllocation->NewerAllocation = IrisAllocation;
            IrisAllocation->NewerAllocation->OlderAllocation = IrisAllocation;
        }

        Allocator->NextAllocation = IrisAllocation->OlderAllocation;
    }
    else
    {
		Allocation = malloc(sizeof(IRIS_MEMORY_ALLOCATION)+ SizeInBytes);

        if (Allocation == NULL)
        {
            return NULL;
        }

		IrisAllocation = (PIRIS_MEMORY_ALLOCATION) Allocation;

        Allocator->AllocationListTail->NewerAllocation = IrisAllocation;
        IrisAllocation->OlderAllocation = Allocator->AllocationListTail;
        IrisAllocation->NewerAllocation = NULL;

        Allocator->AllocationListTail = IrisAllocation;
    }

    return (PCHAR) IrisAllocation + sizeof(IRIS_MEMORY_ALLOCATION);
}

SFORCEINLINE
VOID
IrisMemoryAllocatorFreeAll(
    _Inout_ PIRIS_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = Allocator->AllocationListTail;
}

SFORCEINLINE
VOID
IrisMemoryAllocatorDestroy(
    _Inout_ PIRIS_MEMORY_ALLOCATOR Allocator
    )
{
    PIRIS_MEMORY_ALLOCATION NextAllocation;
    PIRIS_MEMORY_ALLOCATION Temp;

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

#endif _IRIS_MEMORY_ALLOCATOR_INTERNAL_