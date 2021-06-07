/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    alpha_material.c

Abstract:

    Material for doing alpha transparency.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/alpha_material.h"
#include "iris_physx_toolkit/bsdfs/alpha.h"

//
// Types
//

typedef struct _ALPHA_MATERIAL {
    PMATERIAL base;
    PFLOAT_TEXTURE alpha;
} ALPHA_MATERIAL, *PALPHA_MATERIAL;

typedef const ALPHA_MATERIAL *PCALPHA_MATERIAL;

//
// Static Functions
//

static
ISTATUS
AlphaMaterialSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCALPHA_MATERIAL alpha_material = (PCALPHA_MATERIAL)context;

    PCBSDF base_bsdf;
    ISTATUS status = MaterialSample(alpha_material->base,
                                    intersection,
                                    additional_data,
                                    texture_coordinates,
                                    bsdf_allocator,
                                    reflector_compositor,
                                    &base_bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t alpha;
    status = FloatTextureSample(alpha_material->alpha,
                                intersection,
                                additional_data,
                                texture_coordinates,
                                &alpha);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    alpha = IMax((float_t)0.0, IMin(alpha, (float_t)1.0));
    status = AlphaBsdfAllocateWithAllocator(bsdf_allocator,
                                            base_bsdf,
                                            alpha,
                                            bsdf);

    return status;
}

static
void
AlphaMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PALPHA_MATERIAL alpha_material = (PALPHA_MATERIAL)context;

    MaterialRelease(alpha_material->base);
    FloatTextureRelease(alpha_material->alpha);
}

//
// Static Variables
//

static const MATERIAL_VTABLE alpha_material_vtable = {
    AlphaMaterialSample,
    AlphaMaterialFree
};

//
// Functions
//

ISTATUS
AlphaMaterialAllocate(
    _In_ PMATERIAL base_material,
    _In_ PFLOAT_TEXTURE alpha_texture,
    _Out_ PMATERIAL *material
    )
{
    if (base_material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (alpha_texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ALPHA_MATERIAL alpha_material;
    alpha_material.base = base_material;
    alpha_material.alpha = alpha_texture;

    ISTATUS status = MaterialAllocate(&alpha_material_vtable,
                                      &alpha_material,
                                      sizeof(ALPHA_MATERIAL),
                                      alignof(ALPHA_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MaterialRetain(base_material);
    FloatTextureRetain(alpha_texture);

    return ISTATUS_SUCCESS;
}