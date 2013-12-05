/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module implements the SHAPE_HIT_LIST public functions.

--*/

#include <irisp.h>

//
// Defines
//

#define GEOMETRY_HIT_LIST_GROWTH_FACTOR 2
#define GEOMETRY_HIT_LIST_INITIAL_SIZE 5 
#define SHAPE_HIT_LIST_GROWTH_FACTOR 2
#define SHAPE_HIT_LIST_INITIAL_SIZE 5

//
// Functions
//

_Check_return_ 
_Ret_maybenull_
STATIC
PSHAPE_HIT 
ShapeHitCollectionAllocateHitInternal(
    _In_ PVOID Context, 
    _In_ SIZE_T AdditionalDataSize
    )
{
    PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection;
    PGEOMETRY_HIT GeometryHit;
    SIZE_T AllocationSize;
    PVOID Allocation;

    ASSERT(Context != NULL);

    ShapeHitCollection = (PSHAPE_HIT_COLLECTION_INTERNAL) Context;

    AllocationSize = sizeof(GEOMETRY_HIT) + AdditionalDataSize;

    Allocation = IrisMemoryAllocatorAllocate(&ShapeHitCollection->Allocator,
                                             AllocationSize);

    GeometryHit = (PGEOMETRY_HIT) Allocation;

    GeometryHit->SharedGeometryHit = ShapeHitCollection->GeometryHitList;
    
    GeometryHit->SharedGeometryHit += ShapeHitCollection->GeometryListSize;

    if (ShapeHitCollection->GeometryAllocationNeeded == FALSE)
    {
        ASSERT(ShapeHitCollection->GeometryListSize > 0);
        
        GeometryHit->SharedGeometryHit -= 1;
    }

    return &GeometryHit->ShapeHit;
}

_Check_return_ 
_Success_(return == ISTATUS_SUCCESS) 
STATIC
ISTATUS 
ShapeHitCollectionAddHitInternal(
    _In_ PVOID Context, 
    _In_ PSHAPE_HIT ShapeHit
    )
{
    PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection;
    PSHARED_GEOMETRY_HIT NewGeometryHitList;
    SIZE_T NewGeometryHitCapacityInBytes;
    SIZE_T NewGeometryHitCapacity;
    PSHAPE_HIT *NewShapeHitList;
    SIZE_T GeometryHitCapacity;
    SIZE_T GeometryHitSize;
    SIZE_T NewListCapacity;
	PVOID Allocation;

    ASSERT(Context != NULL);
    ASSERT(ShapeHit != NULL);

    ShapeHitCollection = (PSHAPE_HIT_COLLECTION_INTERNAL) Context;

    if (ShapeHitCollection->GeometryAllocationNeeded == TRUE)
    {
        GeometryHitCapacity = ShapeHitCollection->GeometryListCapacity;
        GeometryHitSize = ShapeHitCollection->GeometryListSize;

        if (GeometryHitCapacity == GeometryHitSize)
        {
            NewGeometryHitCapacity = GeometryHitCapacity *
                                     GEOMETRY_HIT_LIST_GROWTH_FACTOR;

            NewGeometryHitList = ShapeHitCollection->GeometryHitList;

            NewGeometryHitCapacityInBytes = NewGeometryHitCapacity * 
                                            sizeof(SHARED_GEOMETRY_HIT);

            Allocation = realloc(NewGeometryHitList,
                                 NewGeometryHitCapacityInBytes);

            if (Allocation == NULL)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }

			NewGeometryHitList = (PSHARED_GEOMETRY_HIT) Allocation;

            ShapeHitCollection->GeometryHitList = NewGeometryHitList;
            ShapeHitCollection->GeometryListCapacity = NewGeometryHitCapacity;
        }

        ShapeHitCollection->GeometryAllocationNeeded = FALSE;
        GeometryHitSize++;
    }

    if (ShapeHitCollection->ListSize == ShapeHitCollection->ListCapacity)
    {
        NewListCapacity = ShapeHitCollection->ListCapacity *
                          SHAPE_HIT_LIST_GROWTH_FACTOR;

        Allocation = realloc(ShapeHitCollection->ShapeHitList,
                             NewListCapacity * sizeof(PSHAPE_HIT));

        if (Allocation == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

		NewShapeHitList = (PSHAPE_HIT*) Allocation;

        ShapeHitCollection->ShapeHitList = NewShapeHitList;
        ShapeHitCollection->ListCapacity = NewListCapacity;
    }

    ShapeHitCollection->ShapeHitList[ShapeHitCollection->ListSize] = ShapeHit;

    return ISTATUS_SUCCESS;
}

STATIC SHAPE_HIT_COLLECTION_VTABLE InternalShapeHitCollectionVTable = {
    ShapeHitCollectionAllocateHitInternal,
    ShapeHitCollectionAddHitInternal
};

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitCollectionInitialize(
    _Out_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    SIZE_T GeometryInitialListSizeInBytes;
    PSHARED_GEOMETRY_HIT GeometryHits;
    SIZE_T InitialListSizeInBytes;
    PSHAPE_HIT *ShapeHitList;
	PVOID Allocation;

    ASSERT(ShapeHitCollection != NULL);

    InitialListSizeInBytes = sizeof(PSHAPE_HIT) * 
                             SHAPE_HIT_LIST_INITIAL_SIZE;

    Allocation = malloc(InitialListSizeInBytes);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

	ShapeHitList = (PSHAPE_HIT*) Allocation;

    GeometryInitialListSizeInBytes = sizeof(SHARED_GEOMETRY_HIT) * 
                                     GEOMETRY_HIT_LIST_INITIAL_SIZE;

    Allocation = malloc(GeometryInitialListSizeInBytes);

    if (Allocation == NULL)
    {
        free(ShapeHitList);
        return ISTATUS_ALLOCATION_FAILED;
    }

	GeometryHits = (PSHARED_GEOMETRY_HIT) Allocation;

    ShapeHitCollection->ShapeHitList = ShapeHitList;
    ShapeHitCollection->ListCapacity = SHAPE_HIT_LIST_INITIAL_SIZE;
    ShapeHitCollection->ListSize = 0;
    ShapeHitCollection->GeometryHitList = GeometryHits;
    ShapeHitCollection->GeometryListCapacity = GEOMETRY_HIT_LIST_INITIAL_SIZE;
    ShapeHitCollection->GeometryListSize = 0;
    ShapeHitCollection->GeometryAllocationNeeded = FALSE;

    IrisMemoryAllocatorInitialize(&ShapeHitCollection->Allocator);

    ShapeHitCollection->VTable = &InternalShapeHitCollectionVTable;

	return ISTATUS_SUCCESS;
}