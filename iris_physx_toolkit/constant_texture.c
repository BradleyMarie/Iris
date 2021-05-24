/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant_texture.c

Abstract:

    Allocates a texture from a 2D image.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/constant_texture.h"

//
// Types
//

typedef struct _CONSTANT_FLOAT_TEXTURE {
    float_t value;
} CONSTANT_FLOAT_TEXTURE, *PCONSTANT_FLOAT_TEXTURE;

typedef const CONSTANT_FLOAT_TEXTURE *PCCONSTANT_FLOAT_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ConstantFloatTextureSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    PCONSTANT_FLOAT_TEXTURE texture = (PCONSTANT_FLOAT_TEXTURE)context;

    *value = texture->value;

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const FLOAT_TEXTURE_VTABLE constant_float_texture_vtable = {
    ConstantFloatTextureSample,
    NULL
};

//
// Functions
//

ISTATUS
ConstantFloatTextureAllocate(
    _In_ float_t value,
    _Out_ PFLOAT_TEXTURE *texture
    )
{
    if (!isfinite(value))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (value == (float_t)0.0)
    {
        *texture = NULL;
        return ISTATUS_SUCCESS;
    }

    CONSTANT_FLOAT_TEXTURE constant_texture;
    constant_texture.value = value;

    ISTATUS status  = FloatTextureAllocate(&constant_float_texture_vtable,
                                           &constant_texture,
                                           sizeof(CONSTANT_FLOAT_TEXTURE),
                                           alignof(CONSTANT_FLOAT_TEXTURE),
                                           texture);

    return status;
}

//
// Types
//

typedef struct _CONSTANT_REFLECTOR_TEXTURE {
    PREFLECTOR reflector;
} CONSTANT_REFLECTOR_TEXTURE, *PCONSTANT_REFLECTOR_TEXTURE;

typedef const CONSTANT_REFLECTOR_TEXTURE *PCCONSTANT_REFLECTOR_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ConstantReflectorTextureSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCCONSTANT_REFLECTOR_TEXTURE texture =
        (PCCONSTANT_REFLECTOR_TEXTURE)context;

    *reflector = texture->reflector;

    return ISTATUS_SUCCESS;
}

static
void
ConstantReflectorTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCONSTANT_REFLECTOR_TEXTURE texture = (PCONSTANT_REFLECTOR_TEXTURE)context;
    ReflectorRelease(texture->reflector);
}

//
// Static Variables
//

static const REFLECTOR_TEXTURE_VTABLE constant_reflector_texture_vtable = {
    ConstantReflectorTextureSample,
    ConstantReflectorTextureFree
};

//
// Functions
//

ISTATUS
ConstantReflectorTextureAllocate(
    _In_opt_ PREFLECTOR reflector,
    _Out_ PREFLECTOR_TEXTURE *texture
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector == NULL)
    {
        *texture = NULL;
        return ISTATUS_SUCCESS;
    }

    CONSTANT_REFLECTOR_TEXTURE constant_texture;
    constant_texture.reflector = reflector;

    ISTATUS status = ReflectorTextureAllocate(&constant_reflector_texture_vtable,
                                              &constant_texture,
                                              sizeof(PCONSTANT_REFLECTOR_TEXTURE),
                                              alignof(PCONSTANT_REFLECTOR_TEXTURE),
                                              texture);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}