/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisadvanced_scene.h

Abstract:

    This file contains the definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_ADVANCED_
#define _SCENE_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSCENE_TRACE_ROUTINE)(
    _In_ PCVOID Context, 
    _Inout_ PRAYTRACER RayTracer
    );

typedef struct _SCENE_VTABLE {
    PSCENE_TRACE_ROUTINE TraceRoutine;
    PFREE_ROUTINE FreeRoutine;
} SCENE_VTABLE, *PSCENE_VTABLE;

typedef CONST SCENE_VTABLE *PCSCENE_VTABLE;

typedef struct _SCENE SCENE, *PSCENE;
typedef CONST SCENE *PCSCENE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISADVANCEDAPI
PSCENE
SceneAllocate(
    _In_ PCSCENE_VTABLE SceneVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS 
SceneTrace(
    _In_ PCSCENE Scene,
    _Inout_ PRAYTRACER RayTracer
    );

IRISADVANCEDAPI
VOID
SceneReference(
    _In_opt_ PSCENE Scene
    );

IRISADVANCEDAPI
VOID
SceneDereference(
    _In_opt_ _Post_invalid_ PSCENE Scene
    );

#endif // _SCENE_IRIS_ADVANCED_