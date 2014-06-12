/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scenetracer.c

Abstract:

    This file contains the function definitions for the SCENE_TRACER type.

--*/

#include <irisshadingmodelp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SceneTracerTraceGeometry(
    _Inout_ PSCENE_TRACER SceneTracer,
    _In_ PCSCENE_OBJECT SceneObject
    )
{
    PGEOMETRY Geometry;
    ISTATUS Status;

    ASSERT(SceneTracer != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(SceneObject != NULL);

    Geometry = (PGEOMETRY) SceneObject;

    Status = RayTracerTraceGeometry(SceneTracer->RayTracer,
                                    Geometry);

    return Status;
}