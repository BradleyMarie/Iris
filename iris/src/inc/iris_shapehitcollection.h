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

typedef struct _SHAPE_HIT_COLLECTION_INTERNAL {
    PSHAPE_HIT_COLLECTION_VTABLE VTable;
    IRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    IRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    PSHARED_GEOMETRY_HIT CurrentGeometryHit;
    IRIS_POINTER_LIST GeometryHitList;
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

    IrisDynamicMemoryAllocatorDestroy(&ShapeHitCollection->AdditionalDataAllocator);
    IrisStaticMemoryAllocatorDestroy(&ShapeHitCollection->GeometryHitAllocator);
    IrisPointerListDestroy(&ShapeHitCollection->GeometryHitList);
}

SFORCEINLINE
VOID
ShapeHitCollectionSetCurrentGeometryHit(
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection,
    _In_ PSHARED_GEOMETRY_HIT CurrentGeometryHit
    )
{
    ASSERT(ShapeHitCollection != NULL);
    ASSERT(CurrentGeometryHit != NULL);

    ShapeHitCollection->CurrentGeometryHit = CurrentGeometryHit;
}

SFORCEINLINE
VOID
ShapeHitCollectionClear(
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    ASSERT(ShapeHitCollection != NULL);

    IrisDynamicMemoryAllocatorFreeAll(&ShapeHitCollection->AdditionalDataAllocator);
    IrisStaticMemoryAllocatorFreeAll(&ShapeHitCollection->GeometryHitAllocator);
    IrisPointerListClear(&ShapeHitCollection->GeometryHitList);
}

#endif // _IRIS_SHAPE_HIT_LIST_INTERNAL_