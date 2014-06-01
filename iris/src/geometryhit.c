/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    geometryhit.c

Abstract:

    This file contains the function definitions for the 
    SHARED_GEOMETRY_HIT type.

--*/

#include <irisp.h>

//
// Functions
//

VOID
SharedGeometryHitComputeModelViewer(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PCVECTOR3 WorldViewer,
    _Out_ PVECTOR3 ModelViewer
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(ModelViewer != NULL);

    if (GeometryHit->ModelToWorld == NULL)
    {
        *ModelViewer = *WorldViewer;
    }
    else if (GeometryHit->Premultiplied != FALSE)
    {
        VectorMatrixTransposedMultiply(GeometryHit->ModelToWorld->Inverse,
                                       WorldViewer,
                                       ModelViewer);
    }
    else
    {
        *ModelViewer = GeometryHit->ModelRay.Direction;
    }
}

VOID
SharedGeometryHitComputeModelHit(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PCPOINT3 WorldHitPoint,
    _Out_ PPOINT3 ModelHitPoint
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);

    if (GeometryHit->ModelToWorld == NULL)
    {
        *ModelHitPoint = *WorldHitPoint;
    }
    else if (GeometryHit->Premultiplied != FALSE)
    {
        PointMatrixMultiply(GeometryHit->ModelToWorld,
                            WorldHitPoint,
                            ModelHitPoint);
    }
    else
    {
        *ModelHitPoint = GeometryHit->ModelRay.Origin;
    }
}

PCMATRIX
SharedGeometryHitGetModelToWorld(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit
    )
{
    ASSERT(GeometryHit != NULL);

    return GeometryHit->ModelToWorld;
}

PCMATRIX
SharedGeometryHitGetWorldToModel(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit
    )
{
    PCMATRIX ModelToWorld;

    ASSERT(GeometryHit != NULL);

    ModelToWorld = GeometryHit->ModelToWorld;

    if (ModelToWorld == NULL)
    {
        return NULL;
    }

    return ModelToWorld->Inverse;
}