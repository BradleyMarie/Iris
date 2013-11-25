/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module implements the SHAPE_HIT struct.

--*/

#ifndef _IRIS_SHAPE_HIT_
#define _IRIS_SHAPE_HIT_

#include <irisp.h>

//
// Types
//

typedef struct _SHAPE_HIT {
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSize) PVOID AdditionalData;
    SIZE_T AdditionalDataSize;
} SHAPE_HIT, *PSHAPE_HIT;

//
// Functions
//

SFORCEINLINE
VOID
ShapeHitInitialize(
    _Out_ PSHAPE_HIT ShapeHit
    )
{
    ASSERT(ShapeHit != NULL);

    ShapeHit->AdditionalData = NULL;
    ShapeHit->AdditionalDataSize = 0;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ShapeHitSet(
    _Inout_ PSHAPE_HIT ShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSize) PVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSize
    )
{
    PVOID NewBuffer;

    ASSERT(ShapeHit != NULL);
    ASSERT(ShapeHit != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(AdditionalDataSize == 0 || AdditionalData != NULL);

    ShapeHit->Distance = Distance;
    ShapeHit->FaceHit = FaceHit;

    if (ShapeHit->AdditionalDataSize < AdditionalDataSize)
    {
        NewBuffer = realloc(ShapeHit->AdditionalData, AdditionalDataSize);

        if (NewBuffer == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        ShapeHit->AdditionalData = NewBuffer;
    }

    if (AdditionalDataSize != 0)
    {
        memcpy(ShapeHit->AdditionalData, AdditionalData, AdditionalDataSize);
    }

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_SHAPE_HIT_