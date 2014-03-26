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
// Static Variables
//

STATIC
CONST
MATRIX IdentityMatrix = { (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          &IdentityMatrix };

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

    switch (GeometryHit->Type)
    {
        case GEOMETRY_TYPE_WORLD:
            *ModelViewer = *WorldViewer;
            break;
        case GEOMETRY_TYPE_PREMULTIPLIED:
            VectorMatrixTransposedMultiply(GeometryHit->ModelToWorld->Inverse,
                                           WorldViewer,
                                           ModelViewer);
            break;
        case GEOMETRY_TYPE_MODEL:
            *ModelViewer = GeometryHit->ModelRay.Direction;
            break;
        default:
            ASSERT(FALSE);
            break;
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

    switch (GeometryHit->Type)
    {
        case GEOMETRY_TYPE_WORLD:
            *ModelHitPoint = *WorldHitPoint;
            break;
        case GEOMETRY_TYPE_PREMULTIPLIED:
            PointMatrixMultiply(GeometryHit->ModelToWorld,
                                WorldHitPoint,
                                ModelHitPoint);
            break;
        case GEOMETRY_TYPE_MODEL:
            *ModelHitPoint = GeometryHit->ModelRay.Origin;
            break;
        default:
            ASSERT(FALSE);
            break;
    }
}

PCMATRIX
SharedGeometryHitGetModelToWorld(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit
    )
{
    ASSERT(GeometryHit != NULL);

    if (GeometryHit->Type == GEOMETRY_TYPE_WORLD)
    {
        return &IdentityMatrix;
    }

    return GeometryHit->ModelToWorld;
}