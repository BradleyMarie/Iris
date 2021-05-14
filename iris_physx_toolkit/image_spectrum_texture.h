/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    image_spectrum_texture.h

Abstract:

    Allocates a spectrum texture from a 2D image.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_

#include "iris_physx_toolkit/mipmap.h"
#include "iris_physx_toolkit/spectrum_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ImageSpectrumTextureAllocate(
    _In_ PSPECTRUM_MIPMAP mipmap,
    _In_ float_t u_offset,
    _In_ float_t v_offset,
    _In_ float_t u_scalar,
    _In_ float_t v_scalar,
    _Out_ PSPECTRUM_TEXTURE *texture
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_IMAGE_TEXTURE_