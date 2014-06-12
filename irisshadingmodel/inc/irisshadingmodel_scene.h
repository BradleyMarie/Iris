/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisshadingmodel_scene.h

Abstract:

    This file contains the definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_SHADING_MODEL_
#define _SCENE_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSCENE_ADD_OBJECT_ROUTINE)(
    _Inout_ PVOID Context,
    _In_ PSCENE_OBJECT SceneObject
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSCENE_TRACE_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ PCRAY WorldRay,
    _Inout_ PSCENE_TRACER SceneTracer
    );

typedef struct _SCENE_VTABLE {
    PSCENE_ADD_OBJECT_ROUTINE AddObjectRoutine;
    PSCENE_TRACE_ROUTINE TraceRoutine;
    PFREE_ROUTINE FreeRoutine;
} SCENE_VTABLE, *PSCENE_VTABLE;

typedef CONST SCENE_VTABLE *PCSCENE_VTABLE;

typedef struct _SCENE {
    PCSCENE_VTABLE SceneVTable;
} SCENE, *PSCENE;

typedef CONST SCENE *PCSCENE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS 
SceneAddObject(
    _Inout_ PSCENE Scene,
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SceneAddWorldObject(
    _Inout_ PSCENE Scene,
    _In_ PCDRAWING_SHAPE DrawingShape
    )
{
    ASSERT(Scene != NULL);
    ASSERT(DrawingShape != NULL);

    return SceneAddObject(Scene, DrawingShape, NULL, TRUE);
}

SFORCEINLINE
VOID
SceneFree(
    _In_opt_ _Post_invalid_ PSCENE Scene
    )
{
    if (Scene == NULL)
    {
        return;
    }

    Scene->SceneVTable->FreeRoutine(Scene);
}

#endif // _SCENE_IRIS_SHADING_MODEL_