/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    triangle.c

Abstract:

    This file contains the definitions for triangle types.

--*/

#include <irisphysxtoolkitp.h>
#include <iriscommon_listscene.h>

//
// Types
//

typedef struct _PHYSX_LIST_SCENE {
    LIST_SCENE Data; 
} PHYSX_LIST_SCENE, *PPHYSX_LIST_SCENE;

typedef CONST PHYSX_LIST_SCENE *PCPHYSX_LIST_SCENE;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxListSceneAddObject(
    _Inout_ PVOID Context,
    _In_ PSPECTRUM_SHAPE SpectrumShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PPHYSX_LIST_SCENE Scene;
    ISTATUS Status;
    PSHAPE Shape;
    
    ASSERT(Context != NULL);
    ASSERT(SpectrumShape != NULL);
    
    Scene = (PPHYSX_LIST_SCENE) Context;
    Shape = (PSHAPE) SpectrumShape;
    
    Status = ListSceneAddObject(&Scene->Data,
                                Shape,
                                ModelToWorld,
                                Premultiplied);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxListSceneTrace(
    _In_ PCVOID Context, 
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference
    )
{
    PPHYSX_LIST_SCENE Scene;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(RayTracerReference != NULL);
    
    Scene = (PPHYSX_LIST_SCENE) Context;
    
    Status = ListSceneTrace(&Scene->Data,
                            RayTracerReference);
    
    return Status;
}

STATIC
VOID
PhysxListSceneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PPHYSX_LIST_SCENE Scene;
    
    ASSERT(Context != NULL);
    
    Scene = (PPHYSX_LIST_SCENE) Context;
    
    ListSceneDestroy(&Scene->Data);
}

//
// Static Variables
//

CONST STATIC SPECTRUM_SCENE_VTABLE PhysxListSceneVTable = {
    { PhysxListSceneTrace, PhysxListSceneFree },
    PhysxListSceneAddObject
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxListSceneAllocate(
    _Out_ PSPECTRUM_SCENE *Scene
    )
{
    PHYSX_LIST_SCENE ListScene;
    ISTATUS Status;
    
    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    Status = ListSceneInitialize(&ListScene.Data);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = SpectrumSceneAllocate(&PhysxListSceneVTable,
                                   &ListScene,
                                   sizeof(PHYSX_LIST_SCENE),
                                   _Alignof(PHYSX_LIST_SCENE),
                                   Scene);
                                   
    if (Status != ISTATUS_SUCCESS)
    {
        ListSceneDestroy(&ListScene.Data);
        return Status;
    }
    
    return ISTATUS_SUCCESS;
}