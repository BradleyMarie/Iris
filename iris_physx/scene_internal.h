/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    scene_internal.h

Abstract:

    The internal routines for sampling from a scene.

--*/

#ifndef _IRIS_PHYSX_SCENE_INTERNAL_
#define _IRIS_PHYSX_SCENE_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/scene_vtable.h"

//
// Types
//

struct _SCENE {
    PCSCENE_VTABLE vtable;
    void *data;
    PEMISSIVE_MATERIAL background;
    atomic_uintmax_t reference_count;
};

#endif // _IRIS_PHYSX_SCENE_INTERNAL_