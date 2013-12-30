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
SceneObjectTracerTrace(
    _Inout_ PSCENE_OBJECT_TRACER Tracer,
    _In_ PSCENE_OBJECT SceneObject,
    _In_ PRAY WorldRay
    )
{
    PSHAPE_HIT HitListBegin;
    PSHAPE_HIT HitListEnd;
    ISTATUS Status;

    ASSERT(Tracer != NULL);
    ASSERT(SceneObject != NULL);

    Status = SceneObjectTraceObject(SceneObject,
                                    WorldRay,
                                    Tracer->ShapeHitAllocator,
                                    Tracer->SharedGeometryHitAllocator,
                                    &HitListBegin,
                                    &HitListEnd);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitListBegin != NULL)
    {
        HitListEnd->NextHit = Tracer->HitList;
        Tracer->HitList = HitListBegin;   
    }

    return ISTATUS_SUCCESS;
}