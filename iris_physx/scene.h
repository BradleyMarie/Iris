/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    scene.h

Abstract:

    A structured representation of the shapes in a scene.

--*/

#ifndef _IRIS_PHYSX_SCENE_
#define _IRIS_PHYSX_SCENE_

#include "iris_physx/scene_vtable.h"

//
// Types
//

typedef struct _SCENE SCENE, *PSCENE;
typedef const SCENE *PCSCENE;

//
// Functions
//

ISTATUS
SceneAllocate(
    _In_ PCSCENE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSCENE *scene
    );

void
SceneRetain(
    _In_opt_ PSCENE scene
    );

void
SceneRelease(
    _In_opt_ _Post_invalid_ PSCENE scene
    );
    
#endif // _IRIS_PHYSX_SCENE_