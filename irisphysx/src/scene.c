/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SPECTRUM_SCENE type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumSceneAllocate(
    _In_ PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM_SCENE *Result
    )
{
    PSPECTRUM_SCENE SpectrumScene;
    ISTATUS Status;

    SpectrumScene = malloc(sizeof(SPECTRUM_SCENE));
    
    if (SpectrumScene == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    Status = PointerListInitialize(&SpectrumScene->Lights);

    if (Status != ISTATUS_SUCCESS)
    {
        free(SpectrumScene);
        return Status;
    }

    Status = SceneAllocate(&SpectrumSceneVTable->SceneVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
                           &SpectrumScene->Scene);

    if (Status != ISTATUS_SUCCESS)
    {
        PointerListDestroy(&SpectrumScene->Lights);
        free(SpectrumScene);
        return Status;
    }
    
    SpectrumScene->ReferenceCount = 1;
    
    *Result = SpectrumScene;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SpectrumSceneAddObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PPBR_SHAPE SpectrumShape,
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

    Scene = SpectrumScene->Scene;

    SpectrumSceneVTable = (PCSPECTRUM_SCENE_VTABLE) SceneGetVTable(Scene);

    Data = SceneGetData(Scene);

    Status = SpectrumSceneVTable->AddObjectRoutine(Data,
                                                   SpectrumShape,
                                                   ModelToWorld,
                                                   Premultiplied);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SpectrumSceneAddLight(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PLIGHT Light
    )
{
    ISTATUS Status;
    
    if (SpectrumScene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    Status = PointerListAddPointer(&SpectrumScene->Lights,
                                   Light);
                                   
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    LightRetain(Light);

    return ISTATUS_SUCCESS;   
}

VOID
SpectrumSceneReference(
    _In_opt_ PSPECTRUM_SCENE SpectrumScene
    )
{
    if (SpectrumScene == NULL)
    {
        return;
    }

    SpectrumScene->ReferenceCount += 1;
}

VOID
SpectrumSceneDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_SCENE SpectrumScene
    )
{
    SIZE_T NumberOfLights;
    SIZE_T Index;
    PLIGHT Light;
    
    if (SpectrumScene == NULL)
    {
        return;
    }

    SpectrumScene->ReferenceCount -= 1;

    if (SpectrumScene->ReferenceCount == 0)
    {
        NumberOfLights = PointerListGetSize(&SpectrumScene->Lights);
        
        for (Index = 0; Index < NumberOfLights; Index++)
        {
            Light = (PLIGHT) PointerListRetrieveAtIndex(&SpectrumScene->Lights,
                                                        Index);
            LightRelease(Light);
        }
        
        PointerListDestroy(&SpectrumScene->Lights);
        SceneRelease(SpectrumScene->Scene);
        free(SpectrumScene);
    } 
}