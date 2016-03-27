/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisshadingmodel_scene.h

Abstract:

    This file contains the definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_SHADING_MODEL_
#define _SCENE_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PCOLOR_SCENE_ADD_OBJECT_ROUTINE)(
    _Inout_ PVOID Context,
    _In_ PSCENE_OBJECT SceneObject
    );

typedef struct _COLOR_SCENE_VTABLE {
    PRAYTRACER_TEST_SHAPES_ROUTINE TestShapesRoutine;
    PFREE_ROUTINE FreeRoutine;
    PCOLOR_SCENE_ADD_OBJECT_ROUTINE AddObjectRoutine;
} COLOR_SCENE_VTABLE, *PCOLOR_SCENE_VTABLE;

typedef CONST COLOR_SCENE_VTABLE *PCCOLOR_SCENE_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PCOLOR_SCENE
ColorSceneAllocate(
    _In_ PCCOLOR_SCENE_VTABLE SceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS 
ColorSceneAddObject(
    _Inout_ PCOLOR_SCENE Scene,
    _In_ PDRAWING_SHAPE DrawingShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ColorSceneAddWorldObject(
    _Inout_ PCOLOR_SCENE Scene,
    _In_ PDRAWING_SHAPE DrawingShape
    )
{
    ASSERT(Scene != NULL);
    ASSERT(DrawingShape != NULL);

    return ColorSceneAddObject(Scene, DrawingShape, NULL, TRUE);
}

IRISSHADINGMODELAPI
VOID
ColorSceneFree(
    _In_opt_ _Post_invalid_ PCOLOR_SCENE Scene
    );

#endif // _SCENE_IRIS_SHADING_MODEL_