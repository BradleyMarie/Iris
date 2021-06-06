/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    plastic_material.c

Abstract:

    Re-implementation of pbrt's plastic material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/plastic_material.h"
#include "iris_physx_toolkit/bsdfs/aggregate.h"
#include "iris_physx_toolkit/bsdfs/lambertian.h"
#include "iris_physx_toolkit/bsdfs/microfacet.h"

//
// Types
//

typedef struct _PLASTIC_MATERIAL {
    PREFLECTOR_TEXTURE diffuse;
    PREFLECTOR_TEXTURE specular;
    PFLOAT_TEXTURE roughness;
    bool remap_roughness;
} PLASTIC_MATERIAL, *PPLASTIC_MATERIAL;

typedef const PLASTIC_MATERIAL *PCPLASTIC_MATERIAL;

//
// Static Functions
//

static
ISTATUS
PlasticMaterialSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCPLASTIC_MATERIAL plastic_material = (PCPLASTIC_MATERIAL)context;

    PCREFLECTOR diffuse;
    ISTATUS status = ReflectorTextureSample(plastic_material->diffuse,
                                            intersection,
                                            additional_data,
                                            texture_coordinates,
                                            reflector_compositor,
                                            &diffuse);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCREFLECTOR specular;
    status = ReflectorTextureSample(plastic_material->specular,
                                    intersection,
                                    additional_data,
                                    texture_coordinates,
                                    reflector_compositor,
                                    &specular);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t roughness;
    status = FloatTextureSample(plastic_material->roughness,
                                intersection,
                                additional_data,
                                texture_coordinates,
                                &roughness);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (plastic_material->remap_roughness)
    {
        roughness = TrowbridgeReitzRoughnessToAlpha(roughness);
    }

    PCBSDF bsdfs[2];
    status = LambertianBsdfAllocateWithAllocator(bsdf_allocator,
                                                 diffuse,
                                                 &bsdfs[0]);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        TrowbridgeReitzDielectricReflectionBsdfAllocateWithAllocator(bsdf_allocator,
                                                                     specular,
                                                                     roughness,
                                                                     roughness,
                                                                     (float_t)1.5,
                                                                     (float_t)1.0,
                                                                     &bsdfs[1]);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = AggregateBsdfAllocateWithAllocator(bsdf_allocator,
                                                bsdfs,
                                                2,
                                                bsdf);

    return status;
}

static
void
PlasticMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPLASTIC_MATERIAL plastic_material = (PPLASTIC_MATERIAL)context;

    ReflectorTextureRelease(plastic_material->diffuse);
    ReflectorTextureRelease(plastic_material->specular);
    FloatTextureRelease(plastic_material->roughness);
}

//
// Static Variables
//

static const MATERIAL_VTABLE plastic_material_vtable = {
    PlasticMaterialSample,
    PlasticMaterialFree
};

//
// Functions
//

ISTATUS
PlasticMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE diffuse,
    _In_opt_ PREFLECTOR_TEXTURE specular,
    _In_opt_ PFLOAT_TEXTURE roughness,
    _In_ bool remap_roughness,
    _Out_ PMATERIAL *material
    )
{
    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (diffuse == NULL && specular == NULL)
    {
        *material = NULL;
        return ISTATUS_SUCCESS;
    }

    PLASTIC_MATERIAL plastic_material;
    plastic_material.diffuse = diffuse;
    plastic_material.specular = specular;
    plastic_material.roughness = roughness;
    plastic_material.remap_roughness = remap_roughness;

    ISTATUS status = MaterialAllocate(&plastic_material_vtable,
                                      &plastic_material,
                                      sizeof(PLASTIC_MATERIAL),
                                      alignof(PLASTIC_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorTextureRetain(diffuse);
    ReflectorTextureRetain(specular);
    FloatTextureRetain(roughness);

    return ISTATUS_SUCCESS;
}