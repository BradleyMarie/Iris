/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehitcollection.h

Abstract:

    This module implements the SHAPE_HIT_COLLECTION public functions.

--*/

#ifndef _IRIS_SHAPE_HIT_COLLECTION_
#define _IRIS_SHAPE_HIT_COLLECTION_

#include <iris.h>

//
// Types
//

typedef 
_Check_return_ 
_Ret_maybenull_ 
PSHAPE_HIT 
(*SHAPE_HIT_COLLECTION_ALLOCATE_HIT)(
    _In_ PVOID Context, 
    _In_ SIZE_T AdditionalDataSize
    );

typedef 
_Check_return_ 
_Success_(return == ISTATUS_SUCCESS) 
ISTATUS 
(*SHAPE_HIT_COLLECTION_ADD_HIT)(
    _In_ PVOID Context, 
    _In_ PSHAPE_HIT ShapeHit
    );

typedef struct _SHAPE_HIT_COLLECTION_VTABLE {
    SHAPE_HIT_COLLECTION_ALLOCATE_HIT AllocateHitRoutine;
    SHAPE_HIT_COLLECTION_ADD_HIT AddHitRoutine;
} SHAPE_HIT_COLLECTION_VTABLE, *PSHAPE_HIT_COLLECTION_VTABLE;

typedef struct _SHAPE_HIT_COLLECTION {
    PSHAPE_HIT_COLLECTION_VTABLE VTable;
} SHAPE_HIT_COLLECTION, *PSHAPE_HIT_COLLECTION;

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHAPE_HIT
ShapeHitCollectionAllocateHit(
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection,
    _In_ SIZE_T AdditionalDataSize
    )
{
    PSHAPE_HIT Hit;

    ASSERT(ShapeHitCollection != NULL);

    Hit = ShapeHitCollection->VTable->AllocateHitRoutine(ShapeHitCollection, 
                                                         AdditionalDataSize);

    return Hit;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ShapeHitCollectionAddHit(
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection,
    _In_ PSHAPE_HIT ShapeHit
    )
{
    ISTATUS Status;

    ASSERT(ShapeHitCollection != NULL);
    ASSERT(ShapeHit != NULL);

    Status = ShapeHitCollection->VTable->AddHitRoutine(ShapeHitCollection, 
                                                       ShapeHit);

    return Status;
}

#endif // _IRIS_SHAPE_HIT_COLLECTION_