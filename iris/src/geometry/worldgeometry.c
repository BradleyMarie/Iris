/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    worldgeometry.c

Abstract:

    This module declares the world model geometry callback.

--*/

#include <irisp.h>

ISTATUS
GeometryTraceShapeCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Out_ PGEOMETRY_HIT GeometryHit,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    SIZE_T InitialListSize;
    ISTATUS Status;
    PSHAPE Shape;

    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(GeometryHit != NULL);
    ASSERT(ShapeHitList != NULL);

    Shape = (PSHAPE) Context;

    InitialListSize = ShapeHitList->ListSize;
    
    Status = Shape->VTable->TraceRoutine(Context, Ray, ShapeHitList);

	if (InitialListSize != ShapeHitList->ListSize)
    {
        GeometryHitSetWorld(GeometryHit);
    }

    return ISTATUS_SUCCESS;
}