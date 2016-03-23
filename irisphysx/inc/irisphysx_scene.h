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
    _In_ PPBR_SHAPE SpectrumShape,
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
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumSceneAllocate(
    _In_ PCSPECTRUM_SCENE_VTABLE SpectrumSceneVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM_SCENE *SpectrumScene
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
SpectrumSceneAddObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PPBR_SHAPE SpectrumShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SpectrumSceneAddWorldObject(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PPBR_SHAPE SpectrumShape
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
SpectrumSceneAddLight(
    _Inout_ PSPECTRUM_SCENE SpectrumScene,
    _In_ PLIGHT Light
    );

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