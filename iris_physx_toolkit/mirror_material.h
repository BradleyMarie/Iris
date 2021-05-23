/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mirror_material.h

Abstract:

    Re-implementation of pbrt's mirror material.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MIRROR_MATERIAL_
#define _IRIS_PHYSX_TOOLKIT_MIRROR_MATERIAL_

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

#endif // _IRIS_PHYSX_TOOLKIT_MIRROR_MATERIAL_