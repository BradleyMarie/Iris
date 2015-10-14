/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SCENE type.

--*/

#include <irisadvancedp.h>

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
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SceneAllocate(
	_In_ PCSCENE_VTABLE SceneVTable,
	_When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
	_When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
	_When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
	_Out_ PSCENE *Scene
	)
{
	BOOL AllocationSuccessful;
	PVOID HeaderAllocation;
	PVOID DataAllocation;
	PSCENE AllocatedScene;

	if (SceneVTable == NULL)
	{
		return ISTATUS_INVALID_ARGUMENT_00;
	}

	if (DataSizeInBytes != 0)
	{
		if (Data == NULL)
		{
			return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
		}

		if (DataAlignment == 0 ||
			DataAlignment & DataAlignment - 1)
		{
			return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
		}

		if (DataSizeInBytes % DataAlignment != 0)
		{
			return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
		}
	}

	if (Scene == NULL)
	{
		return ISTATUS_INVALID_ARGUMENT_04;
	}

	AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(SCENE),
		  											  _Alignof(SCENE),
													  &HeaderAllocation,
													  DataSizeInBytes,
													  DataAlignment,
													  &DataAllocation,
													  NULL);

	if (AllocationSuccessful == FALSE)
	{
		return ISTATUS_ALLOCATION_FAILED;
	}

	AllocatedScene = (PSCENE) HeaderAllocation;

	AllocatedScene->VTable = SceneVTable;
	AllocatedScene->ReferenceCount = 1;
	AllocatedScene->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

	*Scene = AllocatedScene;

    return ISTATUS_SUCCESS;
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

_Ret_
PCSCENE_VTABLE
SceneGetVTable(
    _In_ PCSCENE Scene
    )
{
    if (Scene == NULL)
    {
        return NULL;
    }

    return Scene->VTable;
}

_Check_return_
_Ret_opt_
PVOID
SceneGetData(
    _In_ PSCENE Scene
    )
{
    if (Scene == NULL)
    {
        return NULL;
    }

    return Scene->Data;
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