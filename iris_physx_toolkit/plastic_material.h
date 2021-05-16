/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    plastic_material.h

Abstract:

    Re-implementation of pbrt's plastic material.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PLASTIC_MATERIAL_
#define _IRIS_PHYSX_TOOLKIT_PLASTIC_MATERIAL_

#include "iris_physx_toolkit/float_texture.h"
#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PlasticMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE diffuse,
    _In_opt_ PREFLECTOR_TEXTURE specular,
    _In_opt_ PFLOAT_TEXTURE roughness,
    _In_ bool remap_roughness,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PLASTIC_MATERIAL_