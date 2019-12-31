/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    image_texture.c

Abstract:

    Implements a Fresnel BSDF.

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
    _In_reads_(height * width) float_t textels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_TEXTURE *texture
    )
{
    if (textels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    FLOAT_IMAGE_TEXTURE image_texture;
    ISTATUS status = FloatMipmapAllocate(textels,
                                         width,
                                         height,
                                         wrap_mode,
                                         &image_texture.mipmap);

    if (status != ISTATUS_SUCCESS)
    {
        assert(status == ISTATUS_ALLOCATION_FAILED);
        return status;
    }

    status  = FloatTextureAllocate(&float_image_texture_vtable,
                                   &image_texture,
                                   sizeof(FLOAT_IMAGE_TEXTURE),
                                   alignof(FLOAT_IMAGE_TEXTURE),
                                   texture);

    if (status != ISTATUS_SUCCESS)
    {
        assert(status == ISTATUS_ALLOCATION_FAILED);
        FloatMipmapFree(image_texture.mipmap);
        return status;
    }

    return ISTATUS_SUCCESS;
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
    ReflectorImageTextureFree
};

//
// Functions
//

ISTATUS
ReflectorImageTextureAllocate(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _In_ PCRGB_INTERPOLATOR rgb_interpolator,
    _Inout_opt_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PREFLECTOR_TEXTURE *texture
    )
{
    if (textels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (rgb_interpolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    REFLECTOR_IMAGE_TEXTURE image_texture;
    ISTATUS status = ReflectorMipmapAllocate(textels,
                                             width,
                                             height,
                                             wrap_mode,
                                             rgb_interpolator,
                                             color_integrator,
                                             &image_texture.mipmap);

    if (status != ISTATUS_SUCCESS)
    {
        assert(status == ISTATUS_ALLOCATION_FAILED);
        return status;
    }

    status  = ReflectorTextureAllocate(&reflector_image_texture_vtable,
                                       &image_texture,
                                       sizeof(PREFLECTOR_IMAGE_TEXTURE),
                                       alignof(PREFLECTOR_IMAGE_TEXTURE),
                                       texture);

    if (status != ISTATUS_SUCCESS)
    {
        assert(status == ISTATUS_ALLOCATION_FAILED);
        ReflectorMipmapFree(image_texture.mipmap);
        return status;
    }

    return ISTATUS_SUCCESS;
}