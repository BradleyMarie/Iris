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
    _Out_ PGEOMETRY_HIT GeometryHit,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    PPREMULTIPLIED_GEOMETRY PremultipliedGeometry;
    SIZE_T InitialListSize;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(GeometryHit != NULL);
    ASSERT(ShapeHitList != NULL);

    PremultipliedGeometry = (PPREMULTIPLIED_GEOMETRY) Context;

    InitialListSize = ShapeHitList->ListSize;

    Status = PremultipliedGeometry->Shape->VTable->TraceRoutine(Context,
                                                                Ray,
                                                                ShapeHitList);

	if (InitialListSize != ShapeHitList->ListSize)
    {
		GeometryHitSetPremultiplied(GeometryHit, 
                                    PremultipliedGeometry->ModelToWorld);
    }

    return ISTATUS_SUCCESS;
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