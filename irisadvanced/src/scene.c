/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SCENE type.

--*/

#include <irisadvanced.h>

//
// Types
//

struct _SCENE {
    PCSCENE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

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
    Scene->ReferenceCount = 1;
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
SceneTrace(
    _In_ PCSCENE Scene,
    _Inout_ PRAYTRACER RayTracer
    )
{
    ISTATUS Status;

    ASSERT(Scene != NULL);
    ASSERT(RayTracer != NULL);

    Status = Scene->VTable->TraceRoutine(Scene->Data, RayTracer);

    return Status;
}

VOID
SceneReference(
    _In_opt_ PSCENE Scene
    )
{
    if (Scene == NULL)
    {
        return;
    }

    Scene->ReferenceCount += 1;
}

VOID
SceneDereference(
    _In_opt_ _Post_invalid_ PSCENE Scene
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Scene == NULL)
    {
        return;
    }

    Scene->ReferenceCount -= 1;

    if (Scene->ReferenceCount == 0)
    {
        FreeRoutine = Scene->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Scene->Data);
        }

        IrisAlignedFree(Scene);
    }
}