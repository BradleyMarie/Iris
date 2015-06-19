/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_scene.h

Abstract:

    This file contains the definitions for the SPECTRUM_SCENE base type.

--*/

#ifndef _SCENE_IRIS_PHYSX_
#define _SCENE_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSPECTRUM_SCENE_ADD_OBJECT_ROUTINE)(
    _Inout_ PVOID Context,
    _In_ PSPECTRUM_SHAPE SpectrumShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

typedef struct _SPECTRUM_SCENE_VTABLE {
    SCENE_VTABLE SceneVTable;
    PSPECTRUM_SCENE_ADD_OBJECT_ROUTINE AddObjectRoutine;
} SPECTRUM_SCENE_VTABLE, *PSPECTRUM_SCENE_VTABLE;

typedef CONST SPECTRUM_SCENE_VTABLE *PCSPECTRUM_SCENE_VTABLE;

typedef struct _SPECTRUM_SCENE SPECTRUM_SCENE, *PSPECTRUM_SCENE;
typedef CONST SPECTRUM_SCENE *PCSPECTRUM_SCENE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PSPECTRUM_SCENE
SpectrumSceneAllocate(
    _In_ PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
SpectrumSceneAddObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PSPECTRUM_SHAPE SpectrumShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SpectrumSceneAddWorldObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PSPECTRUM_SHAPE SpectrumShape
    )
{
    ISTATUS Status;

    ASSERT(SpectrumScene != NULL);
    ASSERT(SpectrumShape != NULL);

    Status = SpectrumSceneAddObject(SpectrumScene, 
                                    SpectrumShape, 
                                    NULL, 
                                    TRUE);

    return Status;
}

IRISPHYSXAPI
VOID
SpectrumSceneReference(
    _In_opt_ PSPECTRUM_SCENE SpectrumScene
    );

IRISPHYSXAPI
VOID
SpectrumSceneDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_SCENE SpectrumScene
    );

#endif // _SCENE_IRIS_PHYSX_