/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehitcollection.h

Abstract:

    This module implements the SHAPE_HIT_COLLECTION internal functions.

--*/

#ifndef _IRIS_SHAPE_HIT_LIST_INTERNAL_
#define _IRIS_SHAPE_HIT_LIST_INTERNAL_

#include <irisp.h>

//
// Types
//

extern SHAPE_HIT_COLLECTION_VTABLE InternalShapeHitCollectionVTable;

typedef struct _SHAPE_HIT_COLLECTION_INTERNAL {
    PSHAPE_HIT_COLLECTION_VTABLE VTable;
    IRIS_MEMORY_ALLOCATOR Allocator; 
    _Field_size_(GeometryListCapacity) PSHARED_GEOMETRY_HIT GeometryHitList;
    SIZE_T GeometryListCapacity;
    SIZE_T GeometryListSize;
    BOOL GeometryAllocationNeeded;
    _Field_size_(ListCapacity) PSHAPE_HIT *ShapeHitList;
    SIZE_T ListCapacity;
    SIZE_T ListSize;
} SHAPE_HIT_COLLECTION_INTERNAL, *PSHAPE_HIT_COLLECTION_INTERNAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitCollectionInitialize(
    _Out_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    );

SFORCEINLINE
VOID
ShapeHitCollectionDestroy(
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    ASSERT(ShapeHitCollection != NULL);

    IrisMemoryAllocatorDestroy(&ShapeHitCollection->Allocator);
}

SFORCEINLINE
PSHARED_GEOMETRY_HIT
ShapeHitCollectionNextGeometryHit(
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    PSHARED_GEOMETRY_HIT GeometryHitList;

    ASSERT(ShapeHitCollection != NULL);

    ShapeHitCollection->GeometryAllocationNeeded = TRUE;

    GeometryHitList = ShapeHitCollection->GeometryHitList;

    return GeometryHitList + ShapeHitCollection->GeometryListSize;
}

SFORCEINLINE
VOID
ShapeHitCollectionClear(
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    ASSERT(ShapeHitCollection != NULL);

    IrisMemoryAllocatorFreeAll(&ShapeHitCollection->Allocator);

    ShapeHitCollection->ListSize = 0;
}

#endif // _IRIS_SHAPE_HIT_LIST_INTERNAL_