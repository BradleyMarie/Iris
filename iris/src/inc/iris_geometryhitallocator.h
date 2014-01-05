/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometryhitallocator.h

Abstract:

    This file contains the internal definitions for the 
    GEOMETRY_HIT_ALLOCATOR type.

--*/

#ifndef _GEOMETRY_HIT_ALLOCATOR_IRIS_
#define _GEOMETRY_HIT_ALLOCATOR_IRIS_

#include <irisp.h>

//
// Types
//

typedef struct _GEOMETRY_HIT_ALLOCATOR {
    IRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
} GEOMETRY_HIT_ALLOCATOR, *PGEOMETRY_HIT_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
GeometryHitAllocatorInitialize(
    _Out_ PGEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    ISTATUS Status;

    ASSERT(Allocator != NULL);

    GeometryHitAllocator = &Allocator->GeometryHitAllocator;

    Status = IrisStaticMemoryAllocatorInitialize(GeometryHitAllocator,
                                                 sizeof(GEOMETRY_HIT));

    return Status;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PGEOMETRY_HIT
GeometryHitAllocatorAllocate(
    _Inout_ PGEOMETRY_HIT_ALLOCATOR Allocator,
    _In_ PCSHARED_GEOMETRY_HIT SharedGeometryHit,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    PGEOMETRY_HIT GeometryHit;
    PVOID Allocation;

    ASSERT(Allocator != NULL);

    GeometryHitAllocator = &Allocator->GeometryHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(GeometryHitAllocator);

    GeometryHit = (PGEOMETRY_HIT) Allocation;

    GeometryHitInitialize(GeometryHit, SharedGeometryHit, ShapeHit);

    return GeometryHit;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
GeometryHitAllocatorFreeLastAllocation(
    _Inout_ PGEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;

    ASSERT(Allocator != NULL);

    GeometryHitAllocator = &Allocator->GeometryHitAllocator;

    IrisStaticMemoryAllocatorFreeLastAllocation(GeometryHitAllocator);
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
GeometryHitAllocatorFreeAll(
    _Inout_ PGEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;

    ASSERT(Allocator != NULL);

    GeometryHitAllocator = &Allocator->GeometryHitAllocator;

    IrisStaticMemoryAllocatorFreeAll(GeometryHitAllocator);
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
GeometryHitAllocatorDestroy(
    _Inout_ PGEOMETRY_HIT_ALLOCATOR Allocator
    )
{
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;

    ASSERT(Allocator != NULL);

    GeometryHitAllocator = &Allocator->GeometryHitAllocator;

    IrisStaticMemoryAllocatorDestroy(GeometryHitAllocator);
}

#endif // _GEOMETRY_HIT_ALLOCATOR_IRIS_