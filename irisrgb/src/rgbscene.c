/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    rgbscene.c

Abstract:

    This file contains the function definitions for the RGB_SCENE type.

--*/

#include <irisrgb.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PRGB_SCENE
RgbSceneAllocate(
    _In_ PCRGB_SCENE_VTABLE RgbSceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSCENE Scene;

    Scene = SceneAllocate(&RgbSceneVTable->SceneVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

    return (PRGB_SCENE) Scene;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RgbSceneAddObject(
    _Inout_ PRGB_SCENE RgbScene,
    _In_ PRGB_SHAPE RgbShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PCRGB_SCENE_VTABLE RgbSceneVTable;
    ISTATUS Status;
    PSCENE Scene;
    PVOID Data;

    if (RgbScene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (RgbShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Scene = (PSCENE) RgbScene;

    RgbSceneVTable = (PCRGB_SCENE_VTABLE) SceneGetVTable(Scene);

    Data = SceneGetData(Scene);

    Status = RgbSceneVTable->AddObjectRoutine(Data,
                                              RgbShape,
                                              ModelToWorld,
                                              Premultiplied);

    return Status;
}

VOID
RgbSceneReference(
    _In_opt_ PRGB_SCENE RgbScene
    )
{
    PSCENE Scene;

    Scene = (PSCENE) RgbScene;

    SceneReference(Scene); 
}

VOID
RgbSceneDereference(
    _In_opt_ _Post_invalid_ PRGB_SCENE RgbScene
    )
{
    PSCENE Scene;

    Scene = (PSCENE) RgbScene;

    SceneDereference(Scene);   
}