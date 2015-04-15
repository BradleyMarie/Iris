/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisrgb_rgbscene.h

Abstract:

    This file contains the definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_RGB_
#define _SCENE_IRIS_RGB_

#include <irisrgb.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PRGB_SCENE_ADD_OBJECT_ROUTINE)(
    _Inout_ PVOID Context,
    _In_ PRGB_SHAPE RgbShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

typedef struct _RGB_SCENE_VTABLE {
    SCENE_VTABLE SceneVTable;
    PRGB_SCENE_ADD_OBJECT_ROUTINE AddObjectRoutine;
} RGB_SCENE_VTABLE, *PRGB_SCENE_VTABLE;

typedef CONST RGB_SCENE_VTABLE *PCRGB_SCENE_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISRGBAPI
PRGB_SCENE
RgbSceneAllocate(
    _In_ PCRGB_SCENE_VTABLE RgbSceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS 
RgbSceneAddObject(
    _Inout_ PRGB_SCENE RgbScene,
    _In_ PRGB_SHAPE RgbShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
RgbSceneAddWorldObject(
    _Inout_ PRGB_SCENE RgbScene,
    _In_ PRGB_SHAPE RgbShape
    )
{
    ISTATUS Status;

    ASSERT(RgbScene != NULL);
    ASSERT(RgbShape != NULL);

    Status = RgbSceneAddObject(RgbScene, 
                               RgbShape, 
                               NULL, 
                               TRUE);

    return Status;
}

IRISRGBAPI
VOID
RgbSceneReference(
    _In_opt_ PRGB_SCENE RgbScene
    );

IRISRGBAPI
VOID
RgbSceneDereference(
    _In_opt_ _Post_invalid_ PRGB_SCENE RgbScene
    );

#endif // _SCENE_IRIS_RGB_