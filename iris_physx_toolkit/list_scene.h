/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    list_scene.h

Abstract:

    Creates scene which tests a ray against all of the geometry it contains.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_LIST_SCENE_
#define _IRIS_PHYSX_TOOLKIT_LIST_SCENE_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ListSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_ PSCENE *scene
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_LIST_SCENE_