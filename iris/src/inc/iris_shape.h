/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shape.h

Abstract:

    This file contains the implementation of the internal shape object.

--*/

#ifndef _IRIS_SHAPE_INTERNAL_
#define _IRIS_SHAPE_INTERNAL_

#include <irisp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ShapeTraceShape(
    _In_ PSHAPE Shape, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
    )
{
    ASSERT(Shape != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitCollection != NULL);

    return Shape->VTable->TraceRoutine(Shape, Ray, ShapeHitCollection);
}

#endif // _IRIS_SHAPE_INTERNAL_