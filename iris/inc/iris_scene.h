/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_scene.h

Abstract:

    This file contains the definitions for the SCENE base type.

--*/

#ifndef _SCENE_IRIS_
#define _SCENE_IRIS_

#include <iris.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSCENE_TRACE_ROUTINE)(
    _In_ PVOID Context, 
    _In_ PRAY WorldRay,
    _Inout_ PSCENE_OBJECT_TRACER Tracer
    );

typedef struct _SCENE_VTABLE {
    PSCENE_TRACE_ROUTINE TraceRoutine;
} SCENE_VTABLE, *PSCENE_VTABLE;

typedef struct _SCENE {
    PSCENE_VTABLE VTable;
} SCENE, *PSCENE;

#endif // _SCENE_IRIS_