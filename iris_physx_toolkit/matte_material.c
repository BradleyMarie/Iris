/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    matte_material.c

Abstract:

    Re-implementation of pbrt's matte material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/matte_material.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/oren_nayar_bsdf.h"

//
// Types
//

typedef struct _MATTE_MATERIAL {
    PREFLECTOR_TEXTURE diffuse;
    PFLOAT_TEXTURE sigma;
} MATTE_MATERIAL, *PMATTE_MATERIAL;

typedef const MATTE_MATERIAL *PCMATTE_MATERIAL;

//
// Static Functions
//

static
ISTATUS
MatteMaterialSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCMATTE_MATERIAL matte_material = (PCMATTE_MATERIAL)context;

    PCREFLECTOR reflector;
    ISTATUS status = ReflectorTextureSample(matte_material->diffuse,
                                            intersection,
                                            additional_data,
                                            texture_coordinates,
                                            reflector_compositor,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t sigma;
    status = FloatTextureSample(matte_material->sigma,
                                intersection,
                                additional_data,
                                texture_coordinates,
                                &sigma);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (sigma == (float_t)0.0)
    {
        status = LambertianBsdfAllocateWithAllocator(bsdf_allocator,
                                                     reflector,
                                                     bsdf);
    }
    else
    {
        status = OrenNayarBsdfAllocateWithAllocator(bsdf_allocator,
                                                    reflector,
                                                    sigma,
                                                    bsdf);
    }

    return status;
}

static
void
MatteMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PMATTE_MATERIAL matte_material = (PMATTE_MATERIAL)context;

    ReflectorTextureRelease(matte_material->diffuse);
    FloatTextureRelease(matte_material->sigma);
}

//
// Static Variables
//

static const MATERIAL_VTABLE matte_material_vtable = {
    MatteMaterialSample,
    MatteMaterialFree
};

//
// Functions
//

ISTATUS
MatteMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE diffuse,
    _In_opt_ PFLOAT_TEXTURE sigma,
    _Out_ PMATERIAL *material
    )
{
    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (diffuse == NULL)
    {
        *material = NULL;
        return ISTATUS_SUCCESS;
    }

    MATTE_MATERIAL matte_material;
    matte_material.diffuse = diffuse;
    matte_material.sigma = sigma;

    ISTATUS status = MaterialAllocate(&matte_material_vtable,
                                      &matte_material,
                                      sizeof(MATTE_MATERIAL),
                                      alignof(MATTE_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorTextureRetain(diffuse);
    FloatTextureRetain(sigma);

    return ISTATUS_SUCCESS;
}