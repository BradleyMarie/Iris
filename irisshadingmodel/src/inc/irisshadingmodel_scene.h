/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_scene.h

Abstract:

    This file contains the internal definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_SHADING_MODEL_INTERNAL_
#define _SCENE_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Types
//

struct _SCENE {
    PCSCENE_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SceneTrace(
    _In_ PCSCENE Scene, 
    _In_ PCRAY WorldRay,
    _Inout_ PRAYTRACER RayTracer
    )
{
    SCENE_TRACER SceneTracer;
    ISTATUS Status;

    ASSERT(Scene != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(RayTracer != NULL);

    SceneTracerInitialize(&SceneTracer, RayTracer);

    Status = Scene->VTable->TraceRoutine(Scene->Data,
                                         WorldRay,
                                         &SceneTracer);

    return Status;
}

#endif // _SCENE_IRIS_SHADING_MODEL_INTERNAL_