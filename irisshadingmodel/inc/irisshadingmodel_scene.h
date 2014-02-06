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
    _In_ PCVOID Context, 
    _In_ PCRAY WorldRay,
    _Inout_ PRAYTRACER Tracer
    );

typedef struct _SCENE_VTABLE {
    PSCENE_TRACE_ROUTINE TraceRoutine;
} SCENE_VTABLE, *PSCENE_VTABLE;

typedef CONST SCENE_VTABLE *PCSCENE_VTABLE;

typedef struct _SCENE {
    PCSCENE_VTABLE VTable;
} SCENE, *PSCENE;

typedef CONST SCENE *PCSCENE;

#endif // _SCENE_IRIS_