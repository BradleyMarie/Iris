/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_staticallocator.h

Abstract:

    This module implements the internal static memory allocator used by Iris. 
    This allocator is not intended to be efficient nor is it intend to be a 
    general purpose allocator. It's just intended to be fast and provide 
    allocations of a fixed size.

    The allocator works by simply creating an array of fixed size elements
    When a region of memory is allocated, it just returns the next element
    from the array. It's possible to free the entire list in bulk as well as 
    whichever element was most recently allocated.

--*/

#ifndef _IRIS_STATIC_MEMORY_ALLOCATOR_INTERNAL_
#define _IRIS_STATIC_MEMORY_ALLOCATOR_INTERNAL_

#include <iris.h>
#include "iris_pointerlist.h"

//
// Types
//

typedef struct _IRIS_STATIC_MEMORY_ALLOCATOR {
    IRIS_POINTER_LIST PointerList;
    SIZE_T AllocationSize;
    SIZE_T CurrentSize;
} IRIS_STATIC_MEMORY_ALLOCATOR, *PIRIS_STATIC_MEMORY_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IrisStaticMemoryAllocatorInitialize(
    _Out_ PIRIS_STATIC_MEMORY_ALLOCATOR Allocator,
    _In_ SIZE_T AllocationSize
    )
{
    ISTATUS Status;

    ASSERT(Allocator != NULL);
    ASSERT(AllocationSize != 0);

    Status = IrisPointerListInitialize(&Allocator->PointerList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Allocator->AllocationSize = AllocationSize;
    Allocator->CurrentSize = 0;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Ret_maybenull_
_Post_writable_byte_size_(Allocator->AllocationSize)
SFORCEINLINE
PVOID
IrisStaticMemoryAllocatorAllocate(
    _Inout_ PIRIS_STATIC_MEMORY_ALLOCATOR Allocator
    )
{
    SIZE_T PointerListSize;
    PVOID Allocation;
    ISTATUS Status;

    ASSERT(Allocator != NULL);

    PointerListSize = IrisPointerListGetSize(&Allocator->PointerList);

    if (Allocator->CurrentSize == PointerListSize)
    {
        Allocation = malloc(Allocator->AllocationSize);

        if (Allocation == NULL)
        {
            return NULL;
        }

        Status = IrisPointerListAddPointer(&Allocator->PointerList,
                                           Allocation);

        if (Status != ISTATUS_SUCCESS)
        {
            free(Allocation);
            return NULL;
        }
    }
    else
    {
        Allocation = IrisPointerListRetrieveAtIndex(&Allocator->PointerList,
                                                    Allocator->CurrentSize);
    }

    Allocator->CurrentSize++;

    return Allocation;
}

SFORCEINLINE
VOID
IrisStaticMemoryAllocatorFreeLastAllocation(
    _Inout_ PIRIS_STATIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);
    ASSERT(Allocator->CurrentSize != 0);

    Allocator->CurrentSize--;
}

SFORCEINLINE
VOID
IrisStaticMemoryAllocatorFreeAll(
    _Inout_ PIRIS_STATIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->CurrentSize = 0;
}

SFORCEINLINE
VOID
IrisStaticMemoryAllocatorDestroy(
    _Inout_ PIRIS_STATIC_MEMORY_ALLOCATOR Allocator
    )
{
    SIZE_T NumberOfAllocations;
    PVOID Allocation;
    SIZE_T Index;

    ASSERT(Allocator != NULL);

    NumberOfAllocations = IrisPointerListGetSize(&Allocator->PointerList);

    for (Index = 0; Index < NumberOfAllocations; Index++)
    {
        Allocation = IrisPointerListRetrieveAtIndex(&Allocator->PointerList, 
                                                    Index);

        free(Allocation);
    }

    IrisPointerListDestroy(&Allocator->PointerList);
}

#endif // _IRIS_STATIC_MEMORY_ALLOCATOR_INTERNAL_