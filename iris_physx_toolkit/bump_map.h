/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    bump_map.h

Abstract:

    Allocates a normal map from a bump map texture.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BUMP_MAP_
#define _IRIS_PHYSX_TOOLKIT_BUMP_MAP_

#include "iris_physx/iris_physx.h"
#include "iris_physx_toolkit/float_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
BumpMapAllocate(
    _In_ PFLOAT_TEXTURE texture,
    _Out_ PNORMAL_MAP *normal_map
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_BUMP_MAP_