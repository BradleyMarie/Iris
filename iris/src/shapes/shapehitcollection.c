/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module implements the SHAPE_HIT_LIST public functions.

--*/

#include <irisp.h>

_Check_return_ 
_Ret_maybenull_
STATIC
PSHAPE_HIT 
ShapeHitCollectionAllocateHitInternal(
    _In_ PVOID Context,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection;
    PGEOMETRY_HIT GeometryHit;
    PSHAPE_HIT ShapeHit;
    SIZE_T AllocationSize;
    PVOID Allocation;
    PVOID *Entry;

    ASSERT(Context != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));

    ShapeHitCollection = (PSHAPE_HIT_COLLECTION_INTERNAL) Context;

	ASSERT(ShapeHitCollection->CurrentGeometryHit != NULL);

    AllocationSize = sizeof(GEOMETRY_HIT) + AdditionalDataSize;

    GeometryHitAllocator = &ShapeHitCollection->GeometryHitAllocator;

    Allocation = IrisStaticMemoryAllocatorAllocate(GeometryHitAllocator);

    GeometryHit = (PGEOMETRY_HIT) Allocation;

    GeometryHit->SharedGeometryHit = ShapeHitCollection->CurrentGeometryHit;

    if (AdditionalDataSize != 0)
    {
        AdditionalDataAllocator = &ShapeHitCollection->AdditionalDataAllocator;

        Allocation = IrisDynamicMemoryAllocatorAllocate(AdditionalDataAllocator,
                                                        AdditionalDataSize);   
    }
    else
    {
        Allocation = NULL;
    }

    ShapeHit = &GeometryHit->ShapeHit;

    ShapeHit->Distance = Distance;
    ShapeHit->FaceHit = FaceHit;
    ShapeHit->AdditionalDataSizeInBytes = AdditionalDataSize;
    ShapeHit->AdditionalData = Allocation;

    Entry = IrisPointerListGetNextPointer(&ShapeHitCollection->GeometryHitList);

    if (Entry == NULL)
    {
        return NULL;
    }

    *Entry = (PVOID) GeometryHit;

    return ShapeHit;
}

STATIC SHAPE_HIT_COLLECTION_VTABLE InternalShapeHitCollectionVTable = {
    ShapeHitCollectionAllocateHitInternal
};

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitCollectionInitialize(
    _Out_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    PIRIS_DYNAMIC_MEMORY_ALLOCATOR AdditionalDataAllocator;
    PIRIS_STATIC_MEMORY_ALLOCATOR GeometryHitAllocator;
    ISTATUS Status;

    ASSERT(ShapeHitCollection != NULL);

    Status = IrisPointerListInitialize(&ShapeHitCollection->GeometryHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    GeometryHitAllocator = &ShapeHitCollection->GeometryHitAllocator;

    Status = IrisStaticMemoryAllocatorInitialize(GeometryHitAllocator,
                                                 sizeof(GEOMETRY_HIT));

    if (Status != ISTATUS_SUCCESS)
    {
        IrisPointerListDestroy(&ShapeHitCollection->GeometryHitList);
        return Status;
    }

    AdditionalDataAllocator = &ShapeHitCollection->AdditionalDataAllocator;

    IrisDynamicMemoryAllocatorInitialize(AdditionalDataAllocator);

    ShapeHitCollection->CurrentGeometryHit = NULL;
    ShapeHitCollection->VTable = &InternalShapeHitCollectionVTable;    

	return ISTATUS_SUCCESS;
}