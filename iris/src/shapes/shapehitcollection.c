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

#define SHAPE_HIT_LIST_INITIAL_SIZE 5
#define SHAPE_HIT_LIST_GROWTH_FACTOR 2

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
    SIZE_T AllocationSize;
    PVOID Allocation;

    ASSERT(Context != NULL);

    ShapeHitCollection = (PSHAPE_HIT_COLLECTION_INTERNAL) Context;

    AllocationSize = sizeof(SHAPE_HIT) + AdditionalDataSize;

    Allocation = IrisMemoryAllocatorAllocate(&ShapeHitCollection->Allocator,
                                             AllocationSize);

    return (PSHAPE_HIT) Allocation;
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
    PSHAPE_HIT *NewShapeHitList;
    SIZE_T NewListCapacity;

    ASSERT(Context != NULL);
    ASSERT(ShapeHit != NULL);

    ShapeHitCollection = (PSHAPE_HIT_COLLECTION_INTERNAL) Context;

    if (ShapeHitCollection->ListSize == ShapeHitCollection->ListCapacity)
    {
        NewListCapacity = ShapeHitCollection->ListCapacity *
                          SHAPE_HIT_LIST_GROWTH_FACTOR;

        NewShapeHitList = realloc(ShapeHitCollection->ShapeHitList,
                                  NewListCapacity * sizeof(PSHAPE_HIT));

        if (NewShapeHitList == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

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
    SIZE_T InitialListSizeInBytes;
    PSHAPE_HIT *ShapeHitList;

    ASSERT(ShapeHitCollection != NULL);

    InitialListSizeInBytes = sizeof(PSHAPE_HIT) * SHAPE_HIT_LIST_INITIAL_SIZE;

    ShapeHitList = malloc(InitialListSizeInBytes);

    if (ShapeHitList == NULL)
    {
		return ISTATUS_ALLOCATION_FAILED;
    }

    ShapeHitCollection->ShapeHitList = ShapeHitList;
    ShapeHitCollection->ListCapacity = SHAPE_HIT_LIST_INITIAL_SIZE;
    ShapeHitCollection->ListSize = 0;

    IrisMemoryAllocatorInitialize(&ShapeHitCollection->Allocator);

    ShapeHitCollection->VTable = &InternalShapeHitCollectionVTable;
}