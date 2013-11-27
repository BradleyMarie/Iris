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

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitListAdd(
    _Inout_ PSHAPE_HIT_LIST ShapeHitList,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize,
    _Outptr_opt_result_bytebuffer_(AdditionalDataSize) PVOID *AdditionalData
    )
{
    PSHAPE_HIT NextHit;
    PSHAPE_HIT NewList;
    SIZE_T NewSize;
    ISTATUS Status;

    ASSERT(ShapeHitList != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(AdditionalDataSize == 0 || AdditionalData != NULL);

    if (ShapeHitList->ListSize == ShapeHitList->ListCapacity)
    {
        if (ShapeHitList->ListCapacity == 0)
        {
            NewList = malloc(sizeof(SHAPE_HIT) * SHAPE_HIT_LIST_INITIAL_SIZE);

            if (NewList == NULL)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }

            ShapeHitList->List = NewList;
            ShapeHitList->ListCapacity = SHAPE_HIT_LIST_INITIAL_SIZE;
        }
        else
        {
            NewSize = SHAPE_HIT_LIST_GROWTH_FACTOR * ShapeHitList->ListCapacity;
            NewList = realloc(ShapeHitList->List, sizeof(SHAPE_HIT) * NewSize);

            if (NewList == NULL)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }

            ShapeHitList->List = NewList;
            ShapeHitList->ListCapacity = NewSize;
        }
    }

    NextHit = ShapeHitList->List + ShapeHitList->ListSize;

    if (ShapeHitList->ListSize == ShapeHitList->HitsInitialized)
    {
        ShapeHitInitialize(NextHit);
    }

    Status = ShapeHitSet(NextHit,
                         Distance,
                         FaceHit,
                         AdditionalDataSize,
                         AdditionalData);

    if (Status == ISTATUS_SUCCESS)
    {
        ShapeHitList->ListSize++;
    }

    return Status;
}