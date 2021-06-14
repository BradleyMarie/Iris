/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bvh.h

Abstract:

    Creates a BVH scene.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SCENES_BVH_
#define _IRIS_PHYSX_TOOLKIT_SCENES_BVH_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
BvhSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_opt_(num_shapes) const PMATRIX transforms[],
    _In_reads_opt_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _In_opt_ PENVIRONMENTAL_LIGHT environment,
    _Out_ PSCENE *scene
    );

ISTATUS
BvhAggregateAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_ size_t num_shapes,
    _Out_ PSHAPE *aggregate
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SCENES_BVH_