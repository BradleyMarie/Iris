/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    image_texture.c

Abstract:

    Allocates a texture from a 2D image.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/image_texture.h"
#include "iris_physx_toolkit/uv_texture_coordinate.h"

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
    PCUV_TEXTURE_COORDINATE uv = (PCUV_TEXTURE_COORDINATE)texture_coordinates;

    float_t u = texture->u_offset + uv->uv[0] * texture->u_scalar;
    float_t v = texture->v_offset + uv->uv[1] * texture->v_scalar;

    ISTATUS status;
    if (uv->has_derivatives)
    {
        status = FloatMipmapFilteredLookup(texture->mipmap,
                                           u,
                                           v,
                                           uv->du_dx * texture->u_scalar,
                                           uv->du_dy * texture->u_scalar,
                                           uv->dv_dx * texture->v_scalar,
                                           uv->dv_dy * texture->v_scalar,
                                           value);
    }
    else
    {
        status = FloatMipmapLookup(texture->mipmap,
                                   u,
                                   v,
                                   value);
    }

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
    PCUV_TEXTURE_COORDINATE uv = (PCUV_TEXTURE_COORDINATE)texture_coordinates;

    float_t u = texture->u_offset + uv->uv[0] * texture->u_scalar;
    float_t v = texture->v_offset + uv->uv[1] * texture->v_scalar;

    ISTATUS status;
    if (uv->has_derivatives)
    {
        status = ReflectorMipmapFilteredLookup(texture->mipmap,
                                               u,
                                               v,
                                               uv->du_dx * texture->u_scalar,
                                               uv->du_dy * texture->u_scalar,
                                               uv->dv_dx * texture->v_scalar,
                                               uv->dv_dy * texture->v_scalar,
                                               reflector_compositor,
                                               value);
    }
    else
    {
        status = ReflectorMipmapLookup(texture->mipmap,
                                       u,
                                       v,
                                       reflector_compositor,
                                       value);
    }

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