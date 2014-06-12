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
// Function Definitions
//

SFORCEINLINE
VOID
GeometryHitInitialize(
    _Out_ PGEOMETRY_HIT GeometryHit,
    _In_ RAY WorldRay,
    _In_ PCINTERNAL_SHAPE_HIT ShapeHit
    )
{
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;

    ASSERT(GeometryHit != NULL);
    ASSERT(ShapeHit != NULL);

    SharedGeometryHit = ShapeHit->SharedGeometryHit;

    GeometryHit->ShapeHit = &ShapeHit->ShapeHit;
    GeometryHit->ModelToWorld = SharedGeometryHit->ModelToWorld;

    if (SharedGeometryHit->Premultiplied != FALSE)
    {
        if (ShapeHit->ModelHitPointValid != FALSE)
        {
            GeometryHit->WorldHitPoint = ShapeHit->ModelHitPoint;
        }
        else
        {
            GeometryHit->WorldHitPoint = RayEndpoint(WorldRay, ShapeHit->ShapeHit.Distance);
        }

        GeometryHit->ModelHitPoint = PointMatrixMultiply(SharedGeometryHit->ModelToWorld,
                                                         GeometryHit->WorldHitPoint);
        
        GeometryHit->ModelViewer = VectorMatrixInverseMultiply(SharedGeometryHit->ModelToWorld,
                                                               WorldRay.Direction);
    }
    else
    {
        if (ShapeHit->ModelHitPointValid != FALSE)
        {
            GeometryHit->ModelHitPoint = ShapeHit->ModelHitPoint;
        }
        else
        {
            GeometryHit->ModelHitPoint = RayEndpoint(SharedGeometryHit->ModelRay, 
                                                     ShapeHit->ShapeHit.Distance);
        }

        GeometryHit->ModelViewer = SharedGeometryHit->ModelRay.Direction;

        GeometryHit->WorldHitPoint = RayEndpoint(WorldRay, ShapeHit->ShapeHit.Distance);
    }
}

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_