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
    _In_ RAY WorldRay,
    _In_ PCSHARED_GEOMETRY_HIT SharedGeometryHit,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(SharedGeometryHit != NULL);
    ASSERT(ShapeHit != NULL);

    GeometryHit->ShapeHit = ShapeHit;

    GeometryHit->WorldHitPoint = RayEndpoint(WorldRay, ShapeHit->Distance);
    GeometryHit->WorldViewer = WorldRay.Direction;

    if (SharedGeometryHit->Premultiplied != FALSE)
    {
        GeometryHit->ModelHitPoint = PointMatrixMultiply(SharedGeometryHit->ModelToWorld,
                                                         GeometryHit->WorldHitPoint);

        GeometryHit->ModelViewer = VectorMatrixInverseTransposedMultiply(SharedGeometryHit->ModelToWorld,
                                                                         GeometryHit->WorldViewer);
    }
    else
    {
        GeometryHit->ModelHitPoint = RayEndpoint(SharedGeometryHit->ModelRay, 
                                                 ShapeHit->Distance);

        GeometryHit->ModelViewer = SharedGeometryHit->ModelRay.Direction;
    }

    GeometryHit->ModelToWorld = SharedGeometryHit->ModelToWorld;
}

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_