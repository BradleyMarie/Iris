/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    modelgeometry.c

Abstract:

    This module declares the internal model geometry definitions.

--*/

#include <irisp.h>

STATIC
ISTATUS
ModelGeometryCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION_INTERNAL ShapeHitCollection
    )
{
    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitCollection != NULL);

    return ISTATUS_SUCCESS;
}

STATIC GEOMETRY_VTABLE ModelGeometryVTable = { ModelGeometryCallback };

VOID
GeometryInitializeModelGeometry(
    _Out_ PMODEL_GEOMETRY Geometry,
    _In_ PMATRIX WorldToModel,
    _In_ PSHAPE Shape
    )
{
    Geometry->VTable = &ModelGeometryVTable;
    Geometry->WorldToModel = WorldToModel;
    Geometry->Shape = Shape;
}