/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sharedgeometryhitallocator.h

Abstract:

    This file contains the internal definitions for the 
    SHARED_GEOMETRY_HIT_ALLOCATOR type.

--*/

#ifndef _SHARED_GEOMETRY_HIT_ALLOCATOR_IRIS_
#define _SHARED_GEOMETRY_HIT_ALLOCATOR_IRIS_

#include <irisp.h>

//
// Types
//

typedef struct _SHARED_GEOMETRY_HIT_ALLOCATOR {
    IRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;
} SHARED_GEOMETRY_HIT_ALLOCATOR, *PSHARED_GEOMETRY_HIT_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SharedGeometryHitAllocatorInitialize(
    _Out_ PSHARED_GEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;
    ISTATUS Status;

    ASSERT(Allocator != NULL);

    SharedGeometryHitAllocator = &Allocator->SharedGeometryHitAllocator;

    Status = IrisStaticMemoryAllocatorInitialize(SharedGeometryHitAllocator,
                                                 sizeof(SHARED_GEOMETRY_HIT));

    return Status;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHARED_GEOMETRY_HIT
SharedGeometryHitAllocatorAllocateHit(
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;
    PVOID Allocation;

    ASSERT(Allocator != NULL);

    SharedGeometryHitAllocator = &Allocator->SharedGeometryHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(SharedGeometryHitAllocator);

    return (PSHARED_GEOMETRY_HIT) Allocation;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
SharedGeometryHitAllocatorFreeLastHit(
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;

    ASSERT(Allocator != NULL);

    SharedGeometryHitAllocator = &Allocator->SharedGeometryHitAllocator;

    IrisStaticMemoryAllocatorFreeLastAllocation(SharedGeometryHitAllocator);
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
SharedGeometryHitAllocatorFreeAllHits(
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;

    ASSERT(Allocator != NULL);

    SharedGeometryHitAllocator = &Allocator->SharedGeometryHitAllocator;

    IrisStaticMemoryAllocatorFreeAll(SharedGeometryHitAllocator);
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
SharedGeometryHitAllocatorDestroy(
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR SharedGeometryHitAllocator;

    ASSERT(Allocator != NULL);

    SharedGeometryHitAllocator = &Allocator->SharedGeometryHitAllocator;

    IrisStaticMemoryAllocatorDestroy(SharedGeometryHitAllocator);
}

#endif // _SHARED_GEOMETRY_HIT_ALLOCATOR_IRIS_