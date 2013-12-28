/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sceneobjecttracer.h

Abstract:

    This file contains the definitions for the SCENE_OBJECT_TRACER type.

--*/

#ifndef _SCENE_OBJECT_TRACER_IRIS_
#define _SCENE_OBJECT_TRACER_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _SCENE_OBJECT_TRACER SCENE_OBJECT_TRACER, *PSCENE_OBJECT_TRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SceneObjectTracerTrace(
    _Inout_ PSCENE_OBJECT_TRACER Tracer,
    _In_ PSCENE_OBJECT SceneObject,
    _In_ PRAY WorldRay
    );

#endif // _SCENE_OBJECT_TRACER_IRIS_