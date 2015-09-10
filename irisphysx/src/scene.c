/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SPECTRUM_SCENE type.

--*/

#include <irisphysx.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSPECTRUM_SCENE
SpectrumSceneAllocate(
    _In_ PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSCENE Scene;

    Scene = SceneAllocate(&SpectrumSceneVTable->SceneVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

    return (PSPECTRUM_SCENE) Scene;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SpectrumSceneAddObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PSPECTRUM_SHAPE SpectrumShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable;
    ISTATUS Status;
    PSCENE Scene;
    PVOID Data;

    if (SpectrumScene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (SpectrumShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Scene = (PSCENE) SpectrumScene;

    SpectrumSceneVTable = (PCSPECTRUM_SCENE_VTABLE)SceneGetVTable(Scene);

    Data = SceneGetData(Scene);

    Status = SpectrumSceneVTable->AddObjectRoutine(Data,
                                                   SpectrumShape,
                                                   ModelToWorld,
                                                   Premultiplied);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
SpectrumSceneAddLight(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PLIGHT Light
    )
{
    PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable;
    ISTATUS Status;
    PSCENE Scene;
    PVOID Data;

    if (SpectrumScene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Scene = (PSCENE) SpectrumScene;

    SpectrumSceneVTable = (PCSPECTRUM_SCENE_VTABLE) SceneGetVTable(Scene);

    Data = SceneGetData(Scene);

    Status = SpectrumSceneVTable->AddLightRoutine(Data, Light);

    return Status;   
}

VOID
SpectrumSceneReference(
    _In_opt_ PSPECTRUM_SCENE SpectrumScene
    )
{
    PSCENE Scene;

    Scene = (PSCENE) SpectrumScene;

    SceneReference(Scene); 
}

VOID
SpectrumSceneDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_SCENE SpectrumScene
    )
{
    PSCENE Scene;

    Scene = (PSCENE) SpectrumScene;

    SceneDereference(Scene);   
}