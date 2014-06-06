/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scene.c

Abstract:

    This file contains the function definitions for the SCENE type.

--*/

#include <irisshadingmodelp.h>

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

    Status = Scene->SceneVTable->AddObjectRoutine(Scene,
                                                  SceneObject);

    return Status;
}