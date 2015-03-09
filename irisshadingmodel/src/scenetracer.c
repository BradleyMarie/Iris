/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    scenetracer.c

Abstract:

    This file contains the function definitions for the SCENE_TRACER type.

--*/

#include <irisshadingmodelp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SceneTracerTraceGeometry(
    _Inout_ PSCENE_TRACER SceneTracer,
    _In_ PCSCENE_OBJECT SceneObject
    )
{
    ISTATUS Status;
    PSHAPE Shape;

    ASSERT(SceneObject != NULL);

    //
    // Leaving this as an assert since this function just passes SceneObject
    // through  to RayTracerTraceGeometry, so if it is NULL it will cause
    // to return ISTATUS_INVALID_ARGUMENT without needing an explicit check
    // here.
    //

    if (SceneTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Shape = (PSHAPE) SceneObject->Shape;

    Status = RayTracerTraceShapeWithTransform(SceneTracer->RayTracer,
                                              Shape,
                                              SceneObject->ModelToWorld,
                                              SceneObject->Premultiplied);

    return Status;
}