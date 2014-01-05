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
    PCSHAPE Shape;
    PCINVERTIBLE_MATRIX ModelToWorld;
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
    _In_ PCSCENE_OBJECT SceneObject, 
    _In_ PCRAY WorldRay,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator,
    _Outptr_result_maybenull_ PCSHARED_GEOMETRY_HIT *SharedGeometryHit,
    _Outptr_result_maybenull_ PCSHAPE_HIT *ShapeHitList
    )
{
    PSHARED_GEOMETRY_HIT GeometryHit;
    ISTATUS Status;
    PCRAY TraceRay;

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

    if (SceneObject->ModelToWorld == NULL)
    {
        GeometryHit->Type = GEOMETRY_TYPE_WORLD;
        TraceRay = WorldRay;
    }
	else if (SceneObject->Premultiplied != FALSE)
    {
        GeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;
        GeometryHit->ModelToWorld = &SceneObject->ModelToWorld->Matrix;
        TraceRay = WorldRay;
    }
    else
    {
        GeometryHit->Type = GEOMETRY_TYPE_MODEL;
        GeometryHit->ModelToWorld = &SceneObject->ModelToWorld->Matrix;

        RayMatrixMultiply(&SceneObject->ModelToWorld->Inverse,
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