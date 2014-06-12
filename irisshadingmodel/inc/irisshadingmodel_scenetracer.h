/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisshadingmodel_scenetracer.h

Abstract:

    This file contains the definitions for the SCENE_TRACER type.

--*/

#ifndef _IRIS_SHADING_MODEL_SCENE_TRACER_
#define _IRIS_SHADING_MODEL_SCENE_TRACER_

#include <irisshadingmodel.h>

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
IRISSHADINGMODELAPI
ISTATUS
SceneTracerTraceGeometry(
    _Inout_ PSCENE_TRACER SceneTracer,
    _In_ PCSCENE_OBJECT SceneObject
    );

#endif // _IRIS_SHADING_MODEL_SCENE_TRACER_