/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mirror.h

Abstract:

    Re-implementation of pbrt's mirror material.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MATERIALS_MIRROR_
#define _IRIS_PHYSX_TOOLKIT_MATERIALS_MIRROR_

#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MirrorMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE reflectance,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MATERIALS_MIRROR_