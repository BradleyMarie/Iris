/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    scene_internal.h

Abstract:

    The internal routines for sampling from a scene.

--*/

#ifndef _IRIS_PHYSX_SCENE_INTERNAL_
#define _IRIS_PHYSX_SCENE_INTERNAL_

#include <assert.h>

#include "iris_physx/scene_vtable.h"

//
// Types
//

struct _SCENE {
    PCSCENE_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
ISTATUS
SceneTrace(
    _In_ const struct _SCENE *scene,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    assert(scene != NULL);
    assert(hit_tester != NULL);
    assert(RayValidate(ray));

    ISTATUS status = scene->vtable->trace_routine(scene->data,
                                                  hit_tester,
                                                  ray);

    return status;
}

#endif // _IRIS_PHYSX_SCENE_INTERNAL_