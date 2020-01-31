/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_texture.c

Abstract:

    Allocates a texture from a 2D image.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/image_texture.h"

//
// Types
//

typedef struct _FLOAT_IMAGE_TEXTURE {
    PFLOAT_MIPMAP mipmap;
    float_t u_offset;
    float_t v_offset;
    float_t u_scalar;
    float_t v_scalar;
} FLOAT_IMAGE_TEXTURE, *PFLOAT_IMAGE_TEXTURE;

typedef const FLOAT_IMAGE_TEXTURE *PCFLOAT_IMAGE_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ImageFloatTextureSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    PFLOAT_IMAGE_TEXTURE texture = (PFLOAT_IMAGE_TEXTURE)context;
    const float *uv = (const float*)texture_coordinates;

    float_t u = fma(uv[0], texture->u_scalar, texture->u_offset);
    float_t v = fma(uv[1], texture->v_scalar, texture->v_offset);

    ISTATUS status = FloatMipmapLookup(texture->mipmap,
                                       u,
                                       v,
                                       (float_t)0.0,
                                       (float_t)0.0, 
                                       (float_t)0.0,
                                       (float_t)0.0,
                                       value);

    return status;
}

static
void
ImageFloatTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PFLOAT_IMAGE_TEXTURE texture = (PFLOAT_IMAGE_TEXTURE)context;
    FloatMipmapFree(texture->mipmap);
}

//
// Static Variables
//

static const FLOAT_TEXTURE_VTABLE float_image_texture_vtable = {
    ImageFloatTextureSample,
    ImageFloatTextureFree
};

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

    FLOAT_IMAGE_TEXTURE image_texture;
    image_texture.mipmap = mipmap;
    image_texture.u_offset = u_offset;
    image_texture.v_offset = v_offset;
    image_texture.u_scalar = u_scalar;
    image_texture.v_scalar = v_scalar;

    ISTATUS status = FloatTextureAllocate(&float_image_texture_vtable,
                                          &image_texture,
                                          sizeof(FLOAT_IMAGE_TEXTURE),
                                          alignof(FLOAT_IMAGE_TEXTURE),
                                          texture);

    return status;
}

//
// Types
//

typedef struct _REFLECTOR_IMAGE_TEXTURE {
    PREFLECTOR_MIPMAP mipmap;
    float_t u_offset;
    float_t v_offset;
    float_t u_scalar;
    float_t v_scalar;
} REFLECTOR_IMAGE_TEXTURE, *PREFLECTOR_IMAGE_TEXTURE;

typedef const REFLECTOR_IMAGE_TEXTURE *PCREFLECTOR_IMAGE_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ImageReflectorTextureSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *value
    )
{
    PREFLECTOR_IMAGE_TEXTURE texture = (PREFLECTOR_IMAGE_TEXTURE)context;
    const float *uv = (const float*)texture_coordinates;

    float_t u = fma(uv[0], texture->u_scalar, texture->u_offset);
    float_t v = fma(uv[1], texture->v_scalar, texture->v_offset);

    ISTATUS status = ReflectorMipmapLookup(texture->mipmap,
                                           u,
                                           v,
                                           (float_t)0.0,
                                           (float_t)0.0, 
                                           (float_t)0.0,
                                           (float_t)0.0,
                                           reflector_compositor,
                                           value);

    return status;
}

static
void
ImageReflectorTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PREFLECTOR_IMAGE_TEXTURE texture = (PREFLECTOR_IMAGE_TEXTURE)context;
    ReflectorMipmapFree(texture->mipmap);
}

//
// Static Variables
//

static const REFLECTOR_TEXTURE_VTABLE reflector_image_texture_vtable = {
    ImageReflectorTextureSample,
    ImageReflectorTextureFree
};

//
// Functions
//

ISTATUS
ImageReflectorTextureAllocate(
    _In_ PREFLECTOR_MIPMAP mipmap,
    _In_ float_t u_offset,
    _In_ float_t v_offset,
    _In_ float_t u_scalar,
    _In_ float_t v_scalar,
    _Out_ PREFLECTOR_TEXTURE *texture
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

    REFLECTOR_IMAGE_TEXTURE image_texture;
    image_texture.mipmap = mipmap;
    image_texture.u_offset = u_offset;
    image_texture.v_offset = v_offset;
    image_texture.u_scalar = u_scalar;
    image_texture.v_scalar = v_scalar;

    ISTATUS status = ReflectorTextureAllocate(&reflector_image_texture_vtable,
                                              &image_texture,
                                              sizeof(REFLECTOR_IMAGE_TEXTURE),
                                              alignof(REFLECTOR_IMAGE_TEXTURE),
                                              texture);

    return status;
}