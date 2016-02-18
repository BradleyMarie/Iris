/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SCENE type.

--*/

#include <irisshadingmodelp.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PCOLOR_SCENE
ColorSceneAllocate(
    _In_ PCCOLOR_SCENE_VTABLE SceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    ISTATUS Status;
    PSCENE Scene;

    Status = SceneAllocate(&SceneVTable->SceneVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
                           &Scene);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    return (PCOLOR_SCENE) Scene;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
ColorSceneAddObject(
    _Inout_ PCOLOR_SCENE Scene,
    _In_ PDRAWING_SHAPE DrawingShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PCCOLOR_SCENE_VTABLE SceneVTable;
    PSCENE_OBJECT SceneObject;
    ISTATUS Status;
    PVOID Data;

    if (Scene == NULL ||
        DrawingShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    SceneObject = SceneObjectAllocate(DrawingShape,
                                      ModelToWorld,
                                      Premultiplied);

    if (SceneObject == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SceneVTable = (PCCOLOR_SCENE_VTABLE) SceneGetVTable(Scene);

    Data = SceneGetData(Scene);

    Status = SceneVTable->AddObjectRoutine(Data, SceneObject);

    return Status;
}

VOID
ColorSceneReference(
    _In_opt_ PCOLOR_SCENE ColorScene
    )
{
    PSCENE Scene = (PSCENE) ColorScene;
    SceneRetain(Scene);
}

VOID
ColorSceneDereference(
    _In_opt_ _Post_invalid_ PCOLOR_SCENE ColorScene
    )
{
    PSCENE Scene = (PSCENE) ColorScene;
    SceneRelease(Scene);
}