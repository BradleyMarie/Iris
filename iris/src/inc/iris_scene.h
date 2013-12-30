/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_scene.h

Abstract:

    This file contains the internal definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_INTERNAL_
#define _SCENE_IRIS_INTERNAL_

#include <irisp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SceneTraceScene(
    _In_ PSCENE Scene,
    _In_ PRAY WorldRay,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT *HitList
    )
{
    SCENE_OBJECT_TRACER Tracer;
    ISTATUS Status;

    ASSERT(Scene != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(SharedGeometryHitAllocator != NULL);
    ASSERT(HitList != NULL);

    SceneObjectTracerInitialize(&Tracer,
                                SharedGeometryHitAllocator,
                                ShapeHitAllocator);

    Status = Scene->VTable->TraceRoutine(Scene, WorldRay, &Tracer);  

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *HitList = SceneObjectTracerGetHitList(&Tracer);

    return ISTATUS_SUCCESS;
}

#endif // _SCENE_IRIS_INTERNAL_