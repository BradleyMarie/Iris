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
    _Outptr_result_maybenull_ PSHARED_GEOMETRY_HIT *SharedGeometryHit,
    _Outptr_result_maybenull_ PSHAPE_HIT *ShapeHitList
    )
{
    PSHARED_GEOMETRY_HIT GeometryHit;
    ISTATUS Status;
    PRAY TraceRay;

    ASSERT(SceneObject != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(SharedGeometryHitAllocator != NULL);
    ASSERT(SharedGeometryHit != NULL);
    ASSERT(ShapeHitList != NULL);

    GeometryHit = SharedGeometryHitAllocatorAllocate(SharedGeometryHitAllocator);

    if (GeometryHit == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (SceneObject->Transformation == NULL)
    {
        GeometryHit->Type = GEOMETRY_TYPE_WORLD;
        TraceRay = WorldRay;
    }
	else if (SceneObject->Premultiplied)
    {
        GeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;
        GeometryHit->ModelToWorld = &SceneObject->Transformation->Matrix;
        TraceRay = WorldRay;
    }
    else
    {
        GeometryHit->Type = GEOMETRY_TYPE_MODEL;
        GeometryHit->ModelToWorld = &SceneObject->Transformation->Matrix;

        RayMatrixMultiply(&SceneObject->Transformation->Matrix,
                          WorldRay,
                          &GeometryHit->ModelRay);

        TraceRay = &GeometryHit->ModelRay;
    }

    Status = ShapeTraceShape(SceneObject->Shape,
                             TraceRay,
                             ShapeHitAllocator,
                             ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (*ShapeHitList == NULL)
    {
        SharedGeometryHitAllocatorFreeLastAllocation(SharedGeometryHitAllocator);
        return ISTATUS_SUCCESS;
    }

    *SharedGeometryHit = GeometryHit;

    return ISTATUS_SUCCESS;
}

#endif // _SCENE_OBJECT_IRIS_INTERNAL_