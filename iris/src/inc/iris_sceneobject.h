/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sceneobject.h

Abstract:

    This file contains the internal definitions for the SCENE_OBJECT type.

--*/

#ifndef _SCENE_OBJECT_IRIS_INTERNAL_
#define _SCENE_OBJECT_IRIS_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _SCENE_OBJECT {
    PSHAPE Shape;
    PINVERTIBLE_MATRIX Transformation;
    BOOL Premultiplied;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SceneObjectTraceObject(
    _In_ PSCENE_OBJECT SceneObject, 
    _In_ PRAY WorldRay,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT *HitList
    )
{
    PSHARED_GEOMETRY_HIT SharedGeometryHit;
    PGEOMETRY_HIT GeometryHit;
    PSHAPE_HIT ShapeHit;
    ISTATUS Status;
    PRAY TraceRay;

    ASSERT(SceneObject != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(SharedGeometryHitAllocator != NULL);
    ASSERT(HitList != NULL);

    SharedGeometryHit = SharedGeometryHitAllocatorAllocateHit(SharedGeometryHitAllocator);

    if (SharedGeometryHit == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (SceneObject->Transformation == NULL)
    {
        SharedGeometryHit->Type = GEOMETRY_TYPE_WORLD;
        TraceRay = WorldRay;
    }
	else if (SceneObject->Premultiplied)
    {
        SharedGeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;
        SharedGeometryHit->ModelToWorld = &SceneObject->Transformation->Matrix;
        TraceRay = WorldRay;
    }
    else
    {
        SharedGeometryHit->Type = GEOMETRY_TYPE_MODEL;
        SharedGeometryHit->ModelToWorld = &SceneObject->Transformation->Matrix;

        RayMatrixMultiply(&SceneObject->Transformation->Matrix,
                          WorldRay,
                          &SharedGeometryHit->ModelRay);

        TraceRay = &SharedGeometryHit->ModelRay;
    }

    Status = ShapeTraceShape(SceneObject->Shape,
                             TraceRay,
                             ShapeHitAllocator,
                             HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (HitList == NULL)
    {
        SharedGeometryHitAllocatorFreeLastHit(SharedGeometryHitAllocator);
        return ISTATUS_SUCCESS;
    }

    ShapeHit = *HitList;

    while (ShapeHit != NULL)
    {
        GeometryHit = (PGEOMETRY_HIT) ShapeHit;

        GeometryHit->SharedGeometryHit = SharedGeometryHit;

        ShapeHit = ShapeHit->FartherHit;
    }

    return ISTATUS_SUCCESS;
}

#endif // _SCENE_OBJECT_IRIS_INTERNAL_