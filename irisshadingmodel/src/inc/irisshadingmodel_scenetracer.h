/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisshadingmodel_scenetracer.h

Abstract:

    This file contains the internal definitions for the SCENE_TRACER type.

--*/

#ifndef _IRIS_SHADING_MODEL_SCENE_TRACER_INTERNAL_
#define _IRIS_SHADING_MODEL_SCENE_TRACER_INTERNAL_

#include <irisshadingmodelp.h>

//
// Types
//

struct _SCENE_TRACER {
    PRAYTRACER RayTracer;
};

//
// Functions
//

SFORCEINLINE
VOID
SceneTracerInitialize(
    _Out_ PSCENE_TRACER SceneTracer,
    _In_ PRAYTRACER RayTracer
    )
{
    ASSERT(SceneTracer != NULL);
    ASSERT(RayTracer != NULL);

    SceneTracer->RayTracer = RayTracer;
}

#endif // _IRIS_SHADING_MODEL_SCENE_TRACER_INTERNAL_