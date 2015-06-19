/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sharedhitdataaallocator.h

Abstract:

    This file contains the internal definitions for the 
    SHARED_HIT_DATA_ALLOCATOR type.

--*/

#ifndef _SHARED_HIT_DATA_ALLOCATOR_IRIS_
#define _SHARED_HIT_DATA_ALLOCATOR_IRIS_

#include <irisp.h>

//
// Types
//

typedef struct _SHARED_HIT_DATA_ALLOCATOR {
    STATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;
} SHARED_HIT_DATA_ALLOCATOR, *PSHARED_HIT_DATA_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SharedHitDataAllocatorInitialize(
    _Out_ PSHARED_HIT_DATA_ALLOCATOR Allocator
    )
{
    PSTATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;
    ISTATUS Status;

    ASSERT(Allocator != NULL);

    SharedHitDataAllocator = &Allocator->SharedHitDataAllocator;

    Status = StaticMemoryAllocatorInitialize(SharedHitDataAllocator,
                                             sizeof(SHARED_HIT_DATA));

    return Status;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHARED_HIT_DATA
SharedHitDataAllocatorAllocate(
    _Inout_ PSHARED_HIT_DATA_ALLOCATOR Allocator
    )
{
    PSTATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;
    PVOID Allocation;

    ASSERT(Allocator != NULL);

    SharedHitDataAllocator = &Allocator->SharedHitDataAllocator;

    Allocation = StaticMemoryAllocatorAllocate(SharedHitDataAllocator);

    return (PSHARED_HIT_DATA) Allocation;
}

SFORCEINLINE
VOID
SharedHitDataAllocatorFreeLastAllocation(
    _Inout_ PSHARED_HIT_DATA_ALLOCATOR Allocator
    )
{
    PSTATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;

    ASSERT(Allocator != NULL);

    SharedHitDataAllocator = &Allocator->SharedHitDataAllocator;

    StaticMemoryAllocatorFreeLastAllocation(SharedHitDataAllocator);
}

SFORCEINLINE
VOID
SharedHitDataAllocatorFreeAll(
    _Inout_ PSHARED_HIT_DATA_ALLOCATOR Allocator
    )
{
    PSTATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;

    ASSERT(Allocator != NULL);

    SharedHitDataAllocator = &Allocator->SharedHitDataAllocator;

    StaticMemoryAllocatorFreeAll(SharedHitDataAllocator);
}

SFORCEINLINE
VOID
SharedHitDataAllocatorDestroy(
    _Inout_ PSHARED_HIT_DATA_ALLOCATOR Allocator
    )
{
    PSTATIC_MEMORY_ALLOCATOR SharedHitDataAllocator;

    ASSERT(Allocator != NULL);

    SharedHitDataAllocator = &Allocator->SharedHitDataAllocator;

    StaticMemoryAllocatorDestroy(SharedHitDataAllocator);
}

#endif // _SHARED_HIT_DATA_ALLOCATOR_IRIS_