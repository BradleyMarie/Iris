/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    premultipliedgeometry.c

Abstract:

    This module declares the internal premultiplied geometry definitions.

--*/

#include <irisp.h>

STATIC
ISTATUS
PremultipliedGeometryCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    PSHARED_GEOMETRY_HIT GeometryHit;
    PPREMULTIPLIED_GEOMETRY Geometry;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitCollection != NULL);

    Geometry = (PPREMULTIPLIED_GEOMETRY) Context;

    GeometryHit = ShapeHitCollectionNextGeometryHit(ShapeHitCollection);

    GeometryHit->ModelToWorld = Geometry->ModelToWorld;
    GeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;

    Status = ShapeTraceShape(Geometry->Shape, 
                             Ray, 
                             (PSHAPE_HIT_COLLECTION) ShapeHitCollection);

    return Status;
}

STATIC GEOMETRY_VTABLE PremultipliedVTable = { PremultipliedGeometryCallback };

VOID
GeometryInitializePremultipliedGeometry(
    _Out_ PPREMULTIPLIED_GEOMETRY Geometry,
    _In_ PMATRIX ModelToWorld,
    _In_ PSHAPE Shape
    )
{
    Geometry->VTable = &PremultipliedVTable;
    Geometry->ModelToWorld = ModelToWorld;
    Geometry->Shape = Shape;
}