/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    matte_material.h

Abstract:

    Re-implementation of pbrt's matte material.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_
#define _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_

#include "iris_physx_toolkit/float_texture.h"
#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MatteMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE diffuse,
    _In_ PFLOAT_TEXTURE sigma,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MATTE_MATERIAL_