/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    perlin_textures.h

Abstract:

    Reimplementation of PBRT textures generated using Perlin noise.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PERLIN_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_PERLIN_TEXTURE_

#include "iris_physx_toolkit/float_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
WindyFloatTextureAllocate(
    _In_opt_ PMATRIX texture_to_world,
    _Out_ PFLOAT_TEXTURE *texture
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PERLIN_TEXTURE_