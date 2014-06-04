/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometryhit.h

Abstract:

    This module declares the internal geometry hit type.

--*/

#ifndef _IRIS_GEOMETRY_HIT_INTERNAL_
#define _IRIS_GEOMETRY_HIT_INTERNAL_

#include <irisp.h>

//
// Types
//

typedef struct _SHARED_GEOMETRY_HIT {
    PCMATRIX ModelToWorld;
    BOOL Premultiplied;
    RAY ModelRay;
} SHARED_GEOMETRY_HIT, *PSHARED_GEOMETRY_HIT;

typedef CONST SHARED_GEOMETRY_HIT *PCSHARED_GEOMETRY_HIT;

//
// Function Definitions
//

SFORCEINLINE
VOID
GeometryHitInitialize(
    _Out_ PGEOMETRY_HIT GeometryHit,
    _In_ PCRAY WorldRay,
    _In_ PCSHARED_GEOMETRY_HIT SharedGeometryHit,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(SharedGeometryHit != NULL);
    ASSERT(ShapeHit != NULL);

    RayEndpoint(WorldRay,
                ShapeHit->Distance,
                &GeometryHit->WorldHitPoint);

    GeometryHit->WorldViewer = WorldRay->Direction;
    GeometryHit->ShapeHit = ShapeHit;

    if (SharedGeometryHit->ModelToWorld == NULL)
    {
        GeometryHit->ModelViewer = GeometryHit->WorldViewer;
        GeometryHit->ModelHitPoint = GeometryHit->WorldHitPoint;
        GeometryHit->ModelToWorld = NULL;
        GeometryHit->WorldToModel = NULL;
        return;
    }

    if (SharedGeometryHit->Premultiplied != FALSE)
    {
        PointMatrixMultiply(SharedGeometryHit->ModelToWorld,
                            &GeometryHit->WorldHitPoint,
                            &GeometryHit->ModelHitPoint);

        VectorMatrixTransposedMultiply(SharedGeometryHit->ModelToWorld->Inverse,
                                       &GeometryHit->WorldViewer,
                                       &GeometryHit->ModelViewer);
    }
    else
    {
        GeometryHit->ModelHitPoint = SharedGeometryHit->ModelRay.Origin;
        GeometryHit->ModelViewer = SharedGeometryHit->ModelRay.Direction;
    }

    GeometryHit->ModelToWorld = SharedGeometryHit->ModelToWorld;
    GeometryHit->WorldToModel = SharedGeometryHit->ModelToWorld->Inverse;
}

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_