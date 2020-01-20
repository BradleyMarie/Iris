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
FloatImageTextureAllocate(
    _In_reads_(height * width) float_t textels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_TEXTURE *texture
    );

ISTATUS
ReflectorImageTextureAllocate(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_TEXTURE *texture
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_