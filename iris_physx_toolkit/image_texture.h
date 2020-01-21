/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_texture.h

Abstract:

    Allocates a texture from a 2D image.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_

#include "iris_physx_toolkit/float_texture.h"
#include "iris_physx_toolkit/mipmap.h"
#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ImageFloatTextureAllocate(
    _In_ PFLOAT_MIPMAP mipmap,
    _In_ float_t u_offset,
    _In_ float_t v_offset,
    _In_ float_t u_scalar,
    _In_ float_t v_scalar,
    _Out_ PFLOAT_TEXTURE *texture
    );

ISTATUS
ImageReflectorTextureAllocate(
    _In_ PREFLECTOR_MIPMAP mipmap,
    _In_ float_t u_offset,
    _In_ float_t v_offset,
    _In_ float_t u_scalar,
    _In_ float_t v_scalar,
    _Out_ PREFLECTOR_TEXTURE *texture
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_