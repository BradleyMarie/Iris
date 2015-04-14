/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisrgb_scenetracer.h

Abstract:

    This file contains the definitions for the SCENE_TRACER type.

--*/

#ifndef _IRIS_RGB_SCENE_TRACER_
#define _IRIS_RGB_SCENE_TRACER_

#include <irisrgb.h>

//
// Types
//

typedef struct _SCENE_TRACER SCENE_TRACER, *PSCENE_TRACER;
typedef CONST SCENE_TRACER *PCSCENE_TRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
SceneTracerTraceGeometry(
    _Inout_ PSCENE_TRACER SceneTracer,
    _In_ PCSCENE_OBJECT SceneObject
    );

#endif // _IRIS_RGB_SCENE_TRACER_