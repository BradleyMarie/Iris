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
} FLOAT_IMAGE_TEXTURE, *PFLOAT_IMAGE_TEXTURE;

typedef const FLOAT_IMAGE_TEXTURE *PCFLOAT_IMAGE_TEXTURE;

//
// Static Functions
//

static
ISTATUS
FloatImageTextureSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    PFLOAT_IMAGE_TEXTURE texture = (PFLOAT_IMAGE_TEXTURE)context;
    const float *uv = (const float*)texture_coordinates;

    ISTATUS status = FloatMipmapLookup(texture->mipmap,
                                       uv[0],
                                       uv[1],
                                       (float_t)0.0,
                                       (float_t)0.0, 
                                       (float_t)0.0,
                                       (float_t)0.0,
                                       value);

    return status;
}

static
void
FloatImageTextureFree(
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
    FloatImageTextureSample,
    FloatImageTextureFree
};

//
// Functions
//

ISTATUS
FloatImageTextureAllocate(
    _In_ PFLOAT_MIPMAP mipmap,
    _Out_ PFLOAT_TEXTURE *texture
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    FLOAT_IMAGE_TEXTURE image_texture;
    image_texture.mipmap = mipmap;

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
} REFLECTOR_IMAGE_TEXTURE, *PREFLECTOR_IMAGE_TEXTURE;

typedef const REFLECTOR_IMAGE_TEXTURE *PCREFLECTOR_IMAGE_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ReflectorImageTextureSample(
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

    ISTATUS status = ReflectorMipmapLookup(texture->mipmap,
                                           uv[0],
                                           uv[1],
                                           (float_t)0.0,
                                           (float_t)0.0, 
                                           (float_t)0.0,
                                           (float_t)0.0,
                                           reflector_compositor,
                                           value);

    return status;
}

static
ISTATUS
ReflectorImageTextureColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PREFLECTOR_IMAGE_TEXTURE texture = (PREFLECTOR_IMAGE_TEXTURE)context;

    ISTATUS status = ReflectorMipmapCacheColors(texture->mipmap, color_cache);

    return status;
}

static
void
ReflectorImageTextureFree(
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
    ReflectorImageTextureSample,
    ReflectorImageTextureColors,
    ReflectorImageTextureFree
};

//
// Functions
//

ISTATUS
ReflectorImageTextureAllocate(
    _In_ PREFLECTOR_MIPMAP mipmap,
    _Out_ PREFLECTOR_TEXTURE *texture
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    REFLECTOR_IMAGE_TEXTURE image_texture;
    image_texture.mipmap = mipmap;

    ISTATUS status = ReflectorTextureAllocate(&reflector_image_texture_vtable,
                                              &image_texture,
                                              sizeof(PREFLECTOR_IMAGE_TEXTURE),
                                              alignof(PREFLECTOR_IMAGE_TEXTURE),
                                              texture);

    return status;
}