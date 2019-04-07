/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    kd_tree_scene.h

Abstract:

    Creates a kd-tree scene.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_KD_TREE_SCENE_
#define _IRIS_PHYSX_TOOLKIT_KD_TREE_SCENE_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _KD_TREE_SCENE KD_TREE_SCENE, *PKD_TREE_SCENE;
typedef const KD_TREE_SCENE *PCKD_TREE_SCENE;

//
// Functions
//

ISTATUS
KdTreeSceneAllocate(
    _In_reads_(num_shapes) PSHAPE const *shapes,
    _In_reads_(num_shapes) PMATRIX const *transforms,
    _In_reads_(num_shapes) const bool *premultiplied,
    _In_ size_t num_shapes,
    _Out_ PKD_TREE_SCENE *kd_tree_scene
    );

void
KdTreeSceneFree(
    _In_opt_ _Post_invalid_ PKD_TREE_SCENE kd_tree_scene
    );

//
// Callback Functions
//

ISTATUS 
KdTreeSceneTraceCallback(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_KD_TREE_SCENE_