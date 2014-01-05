/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    sceneobjecttracer.c

Abstract:

    This module implements the PSCENE_OBJECT_TRACER functions.

--*/

#include <irisp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SceneObjectTracerTraceObject(
    _Inout_ PSCENE_OBJECT_TRACER Tracer,
    _In_ PCSCENE_OBJECT SceneObject,
    _In_ PCRAY WorldRay
    )
{
	PSHARED_GEOMETRY_HIT SharedGeometryHit;
	PSHAPE_HIT_LIST ShapeHitList;
    PCGEOMETRY_HIT GeometryHit;
    ISTATUS Status;

    ASSERT(Tracer != NULL);
    ASSERT(SceneObject != NULL);

    Status = SceneObjectTraceObject(SceneObject,
                                    WorldRay,
                                    Tracer->ShapeHitAllocator,
                                    Tracer->SharedGeometryHitAllocator,
                                    &SharedGeometryHit,
                                    &ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    while (ShapeHitList != NULL)
    {
        GeometryHit = GeometryHitAllocatorAllocate(Tracer->GeometryHitAllocator,
                                                   SharedGeometryHit,
                                                   ShapeHitList->ShapeHit);

        if (GeometryHit == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        Status = IrisConstantPointerListAddPointer(Tracer->HitList,
                                                   GeometryHit);
    }

    return ISTATUS_SUCCESS;
}