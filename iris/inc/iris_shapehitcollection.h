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
(*SHAPE_HIT_COLLECTION_ADD_HIT)(
    _In_ PVOID Context, 
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize
    );

typedef struct _SHAPE_HIT_COLLECTION_VTABLE {
    SHAPE_HIT_COLLECTION_ADD_HIT AddHitRoutine;
} SHAPE_HIT_COLLECTION_VTABLE, *PSHAPE_HIT_COLLECTION_VTABLE;

typedef struct _SHAPE_HIT_COLLECTION {
    PSHAPE_HIT_COLLECTION_VTABLE VTable;
} SHAPE_HIT_COLLECTION, *PSHAPE_HIT_COLLECTION;

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHAPE_HIT
ShapeHitCollectionAddHit(
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize
    )
{
    PSHAPE_HIT Hit;

    ASSERT(ShapeHitCollection != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));

    Hit = ShapeHitCollection->VTable->AddHitRoutine(ShapeHitCollection,
                                                    Distance,
                                                    FaceHit,
                                                    AdditionalDataSize);

    return Hit;
}

#endif // _IRIS_SHAPE_HIT_COLLECTION_