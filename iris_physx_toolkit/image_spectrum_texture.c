/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    image_spectrum_texture.c

Abstract:

    Allocates a texture from a 2D image.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/image_spectrum_texture.h"

//
// Types
//

typedef struct _SPECTRUM_IMAGE_TEXTURE {
    PSPECTRUM_MIPMAP mipmap;
    float_t u_offset;
    float_t v_offset;
    float_t u_scalar;
    float_t v_scalar;
} SPECTRUM_IMAGE_TEXTURE, *PSPECTRUM_IMAGE_TEXTURE;

typedef const SPECTRUM_IMAGE_TEXTURE *PCSPECTRUM_IMAGE_TEXTURE;

//
// Static Functions
//

static
inline
void
ImageSpectrumToUV(
    _In_ VECTOR3 direction,
    _Out_ float uv[2]
    )
{
    direction = VectorNormalize(direction, NULL, NULL);

    float_t clamped_z =
        IMin(IMax(direction.z, (float_t)0.0), (float_t)1.0);
    float_t theta = acos(clamped_z);

    float_t phi = atan2(direction.y, direction.x);
    if (phi < (float_t)0.0) {
        phi += iris_two_pi;
    }

    uv[0] = phi * iris_inv_two_pi;
    uv[1] = theta * iris_inv_pi;
}

static
ISTATUS
ImageSpectrumTextureSample(
    _In_ const void *context,
    _In_ VECTOR3 direction,
    _In_opt_ VECTOR3 dxdy[2],
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *value
    )
{
    PSPECTRUM_IMAGE_TEXTURE texture = (PSPECTRUM_IMAGE_TEXTURE)context;

    float_t uv[2];
    ImageSpectrumToUV(direction, uv);

    ISTATUS status;
    if (dxdy != NULL)
    {
        float_t dx_uv[2];
        ImageSpectrumToUV(dxdy[0], dx_uv);

        float_t dy_uv[2];
        ImageSpectrumToUV(dxdy[1], dy_uv);

        status = SpectrumMipmapFilteredLookup(texture->mipmap,
                                              uv[0],
                                              uv[1],
                                              dx_uv[0] - uv[0],
                                              dy_uv[0] - uv[0],
                                              dx_uv[1] - uv[1],
                                              dy_uv[1] - uv[1],
                                              spectrum_compositor,
                                              value);
    }
    else
    {
        status = SpectrumMipmapLookup(texture->mipmap,
                                      uv[0],
                                      uv[1],
                                      spectrum_compositor,
                                      value);
    }

    return status;
}

static
void
ImageSpectrumTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PSPECTRUM_IMAGE_TEXTURE texture = (PSPECTRUM_IMAGE_TEXTURE)context;
    SpectrumMipmapFree(texture->mipmap);
}

//
// Static Variables
//

static const SPECTRUM_TEXTURE_VTABLE spectrum_image_texture_vtable = {
    ImageSpectrumTextureSample,
    ImageSpectrumTextureFree
};

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
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(u_offset))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(v_offset))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(u_scalar))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(v_scalar))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    SPECTRUM_IMAGE_TEXTURE image_texture;
    image_texture.mipmap = mipmap;
    image_texture.u_offset = u_offset;
    image_texture.v_offset = v_offset;
    image_texture.u_scalar = u_scalar;
    image_texture.v_scalar = v_scalar;

    ISTATUS status = SpectrumTextureAllocate(&spectrum_image_texture_vtable,
                                              &image_texture,
                                              sizeof(SPECTRUM_IMAGE_TEXTURE),
                                              alignof(SPECTRUM_IMAGE_TEXTURE),
                                              texture);

    return status;
}