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
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    PSHARED_GEOMETRY_HIT GeometryHit;
    ISTATUS Status;
    PSHAPE Shape;

    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitCollection != NULL);

    Shape = (PSHAPE) Context;

    GeometryHit = ShapeHitCollectionNextGeometryHit(ShapeHitCollection);

    GeometryHit->Type = GEOMETRY_TYPE_WORLD;

    Status = ShapeTraceShape(Shape, 
                             Ray, 
                             (PSHAPE_HIT_COLLECTION) ShapeHitCollection);

    return Status;
}