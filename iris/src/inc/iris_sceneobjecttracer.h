/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sceneobjecttracer.h

Abstract:

    This file contains the internal definitions for SCENE_OBJECT_TRACER.

--*/

#ifndef _SCENE_OBJECT_TRACER_IRIS_INTERNAL_
#define _SCENE_OBJECT_TRACER_IRIS_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _SCENE_OBJECT_TRACER {
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PSHAPE_HIT HitList;
};

//
// Functions
//

SFORCEINLINE
VOID
SceneObjectTracerInitialize(
    _Out_ PSCENE_OBJECT_TRACER Tracer,
    _In_ PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator,
    _In_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator
    )
{
    ASSERT(Tracer != NULL);
    ASSERT(SharedGeometryHitAllocator != NULL);
    ASSERT(ShapeHitAllocator != NULL);

    Tracer->SharedGeometryHitAllocator = SharedGeometryHitAllocator;
    Tracer->ShapeHitAllocator = ShapeHitAllocator;
    Tracer->HitList = NULL;
}

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHAPE_HIT
SceneObjectTracerGetHitList(
    _In_ PSCENE_OBJECT_TRACER Tracer
    )
{
    ASSERT(Tracer != NULL);

    return Tracer->HitList;
}

#endif // _SCENE_OBJECT_TRACER_IRIS_INTERNAL_