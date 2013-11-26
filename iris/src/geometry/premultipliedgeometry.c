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
    _In_ PVOID GeometryHitList,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    return ISTATUS_SUCCESS;
}

STATIC GEOMETRY_VTABLE PremultipliedVTable = { PremultipliedGeometryCallback };

VOID
GeometryInitializePremultipliedGeometry(
    _Out_ PREMULTIPLIED_GEOMETRY Geometry,
    _In_ PMATRIX ModelToWorld,
    _In_ PSHAPE Shape
    )
{
    Geometry->VTable = &PremultipliedVTable;
    Geometry->ModelToWorld = ModelToWorld;
    Geometry->Shape = Shape;
}