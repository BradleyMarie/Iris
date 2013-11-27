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
    _Out_ PGEOMETRY_HIT GeometryHit,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    PMODEL_GEOMETRY ModelGeometry;
    SIZE_T InitialListSize;
    ISTATUS Status;
    RAY ModelRay;

    ASSERT(Context != NULL);
    ASSERT(Ray != NULL);
    ASSERT(GeometryHit != NULL);
    ASSERT(ShapeHitList != NULL);

    ModelGeometry = (PMODEL_GEOMETRY) Context;

    InitialListSize = ShapeHitList->ListSize;
    
    RayMatrixMultiply(ModelGeometry->WorldToModel, Ray, &ModelRay);

    Status = ModelGeometry->Shape->VTable->TraceRoutine(Context,
                                                        &ModelRay,
                                                        ShapeHitList);

	if (InitialListSize != ShapeHitList->ListSize)
    {
        GeometryHitSetModel(GeometryHit,
                            ModelGeometry->WorldToModel->Inverse,
                            &ModelRay);
    }

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