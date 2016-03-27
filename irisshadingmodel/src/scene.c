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
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PCOLOR_SCENE AllocatedScene;

    if (SceneVTable == NULL)
    {
        return NULL;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return NULL;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return NULL;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return NULL;
        }
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(COLOR_SCENE),
                                                      _Alignof(COLOR_SCENE),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return NULL;
    }

    AllocatedScene = (PCOLOR_SCENE)HeaderAllocation;

    AllocatedScene->VTable = SceneVTable;
    AllocatedScene->ReferenceCount = 1;
    AllocatedScene->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return AllocatedScene;
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
    PSCENE_OBJECT SceneObject;
    ISTATUS Status;

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

    Status = Scene->VTable->AddObjectRoutine(Scene->Data, SceneObject);

    return Status;
}

VOID
ColorSceneFree(
    _In_opt_ _Post_invalid_ PCOLOR_SCENE ColorScene
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (ColorScene == NULL)
    {
        return;
    }

    FreeRoutine = ColorScene->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(ColorScene->Data);
    }

    IrisAlignedFree(ColorScene);
}