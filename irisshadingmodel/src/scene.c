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
PSCENE
SceneAllocate(
    _In_ PCSCENE_VTABLE SceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSCENE Scene;

    if (SceneVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(SCENE),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Scene = (PSCENE) HeaderAllocation;

    Scene->VTable = SceneVTable;
    Scene->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Scene;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SceneAddObject(
    _Inout_ PSCENE Scene,
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PSCENE_OBJECT SceneObject;
    ISTATUS Status;

    ASSERT(DrawingShape != NULL);

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
SceneFree(
    _In_opt_ _Post_invalid_ PSCENE Scene
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Scene == NULL)
    {
        return;
    }

    FreeRoutine = Scene->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Scene->Data);
    }

    IrisAlignedFree(Scene);
}