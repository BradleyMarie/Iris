/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    product_texture.c

Abstract:

    Allocates a product texture.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/product_texture.h"

//
// Types
//

typedef struct _PRODUCT_FLOAT_TEXTURE {
    PFLOAT_TEXTURE multiplicand0;
    PFLOAT_TEXTURE multiplicand1;
} PRODUCT_FLOAT_TEXTURE, *PPRODUCT_FLOAT_TEXTURE;

typedef const PRODUCT_FLOAT_TEXTURE *PCPRODUCT_FLOAT_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ProductFloatTextureSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    PPRODUCT_FLOAT_TEXTURE texture = (PPRODUCT_FLOAT_TEXTURE)context;

    float_t multiplicand;
    ISTATUS status = FloatTextureSample(texture->multiplicand0,
                                        intersection,
                                        additional_data,
                                        texture_coordinates,
                                        &multiplicand);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = FloatTextureSample(texture->multiplicand1,
                                intersection,
                                additional_data,
                                texture_coordinates,
                                value);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *value *= multiplicand;

    return ISTATUS_SUCCESS;
}

static
void
ProductFloatTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPRODUCT_FLOAT_TEXTURE texture = (PPRODUCT_FLOAT_TEXTURE)context;
    FloatTextureRelease(texture->multiplicand0);
    FloatTextureRelease(texture->multiplicand1);
}

//
// Static Variables
//

static const FLOAT_TEXTURE_VTABLE product_float_texture_vtable = {
    ProductFloatTextureSample,
    ProductFloatTextureFree
};

//
// Functions
//

ISTATUS
ProductFloatTextureAllocate(
    _In_opt_ PFLOAT_TEXTURE multiplicand0,
    _In_opt_ PFLOAT_TEXTURE multiplicand1,
    _Out_ PFLOAT_TEXTURE *product
    )
{
    if (product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (multiplicand0 == NULL || multiplicand1 == NULL)
    {
        *product = NULL;
        return ISTATUS_SUCCESS;
    }

    PRODUCT_FLOAT_TEXTURE product_texture;
    product_texture.multiplicand0 = multiplicand0;
    product_texture.multiplicand1 = multiplicand1;

    ISTATUS status = FloatTextureAllocate(&product_float_texture_vtable,
                                          &product_texture,
                                          sizeof(PRODUCT_FLOAT_TEXTURE),
                                          alignof(PRODUCT_FLOAT_TEXTURE),
                                          product);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    FloatTextureRetain(multiplicand0);
    FloatTextureRetain(multiplicand1);

    return ISTATUS_SUCCESS;
}

//
// Types
//

typedef struct _PRODUCT_REFLECTOR_TEXTURE {
    PREFLECTOR_TEXTURE multiplicand0;
    PREFLECTOR_TEXTURE multiplicand1;
} PRODUCT_REFLECTOR_TEXTURE, *PPRODUCT_REFLECTOR_TEXTURE;

typedef const PRODUCT_REFLECTOR_TEXTURE *PCPRODUCT_REFLECTOR_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ProductReflectorTextureSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCPRODUCT_REFLECTOR_TEXTURE texture =
        (PCPRODUCT_REFLECTOR_TEXTURE)context;

    PCREFLECTOR multiplicand0;
    ISTATUS status = ReflectorTextureSample(texture->multiplicand0,
                                            intersection,
                                            additional_data,
                                            texture_coordinates,
                                            reflector_compositor,
                                            &multiplicand0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCREFLECTOR multiplicand1;
    status = ReflectorTextureSample(texture->multiplicand1,
                                    intersection,
                                    additional_data,
                                    texture_coordinates,
                                    reflector_compositor,
                                    &multiplicand1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorMultiplyReflectors(reflector_compositor,
                                                   multiplicand0,
                                                   multiplicand1,
                                                   reflector);

    return status;
}

static
void
ProductReflectorTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPRODUCT_REFLECTOR_TEXTURE texture = (PPRODUCT_REFLECTOR_TEXTURE)context;
    ReflectorTextureRelease(texture->multiplicand0);
    ReflectorTextureRelease(texture->multiplicand1);
}

//
// Static Variables
//

static const REFLECTOR_TEXTURE_VTABLE product_reflector_texture_vtable = {
    ProductReflectorTextureSample,
    ProductReflectorTextureFree
};

//
// Functions
//

ISTATUS
ProductReflectorTextureAllocate(
    _In_opt_ PREFLECTOR_TEXTURE multiplicand0,
    _In_opt_ PREFLECTOR_TEXTURE multiplicand1,
    _Out_ PREFLECTOR_TEXTURE *product
    )
{
    if (product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (multiplicand0 == NULL || multiplicand1 == NULL)
    {
        *product = NULL;
        return ISTATUS_SUCCESS;
    }

    PRODUCT_REFLECTOR_TEXTURE product_texture;
    product_texture.multiplicand0 = multiplicand0;
    product_texture.multiplicand1 = multiplicand1;

    ISTATUS status = ReflectorTextureAllocate(&product_reflector_texture_vtable,
                                              &product_texture,
                                              sizeof(PRODUCT_REFLECTOR_TEXTURE),
                                              alignof(PRODUCT_REFLECTOR_TEXTURE),
                                              product);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorTextureRetain(multiplicand0);
    ReflectorTextureRetain(multiplicand1);

    return ISTATUS_SUCCESS;
}