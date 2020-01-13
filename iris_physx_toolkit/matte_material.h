/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    matte_material.h

Abstract:

    Re-implementation of pbrt's matte material.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_
#define _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_

#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MatteMaterialAllocate(
    _In_ PREFLECTOR_TEXTURE diffuse,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_