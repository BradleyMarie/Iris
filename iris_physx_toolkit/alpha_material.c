/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    alpha_material.c

Abstract:

    Material for doing alpha transparency.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/alpha_material.h"
#include "iris_physx_toolkit/uniform_reflector.h"

//
// Alpha BSDF Types
//

typedef struct _ALPHA_BSDF {
    PCBSDF base;
    PCREFLECTOR reflector;
    float_t alpha;
} ALPHA_BSDF, *PALPHA_BSDF;

typedef const ALPHA_BSDF *PCALPHA_BSDF;

//
// Alpha BSDF Static Functions
//

static
ISTATUS
AlphaBsdfSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCALPHA_BSDF alpha_bsdf = (PCALPHA_BSDF)context;

    float value;
    ISTATUS status = RandomGenerateFloat(rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &value);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (alpha_bsdf->alpha < value)
    {
        float_t falloff =
            fabs(VectorDotProduct(shading_normal, incoming));
        float_t inv_falloff = (float_t)1.0 / falloff;

        float_t passthrough_amount = (float_t)1.0 - alpha_bsdf->alpha;

        status = ReflectorCompositorAttenuateReflector(compositor,
                                                       alpha_bsdf->reflector,
                                                       passthrough_amount * inv_falloff,
                                                       reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *type = BSDF_SAMPLE_TYPE_TRANSMISSION_CONTAINS_SPECULAR;
        *outgoing = incoming;
        *pdf = passthrough_amount;
        return ISTATUS_SUCCESS;
    }

    status = BsdfSample(alpha_bsdf->base,
                        incoming,
                        surface_normal,
                        shading_normal,
                        rng,
                        compositor,
                        reflector,
                        type,
                        outgoing,
                        pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*pdf <= (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   alpha_bsdf->reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (isinf(*pdf))
    {
        *pdf = alpha_bsdf->alpha;
    }
    else
    {
        *pdf *= alpha_bsdf->alpha;
    }

    return status;
}

static
ISTATUS
AlphaBsdfSampleDiffuse(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCALPHA_BSDF alpha_bsdf = (PCALPHA_BSDF)context;

    if (alpha_bsdf->alpha == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = BsdfSampleDiffuse(alpha_bsdf->base,
                                       incoming,
                                       surface_normal,
                                       shading_normal,
                                       rng,
                                       compositor,
                                       reflector,
                                       type,
                                       outgoing,
                                       pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   alpha_bsdf->reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

    return status;
}

static
ISTATUS
AlphaBsdfComputeDiffuse(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCALPHA_BSDF alpha_bsdf = (PCALPHA_BSDF)context;

    if (alpha_bsdf->alpha == (float_t)0.0)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = BsdfComputeDiffuse(alpha_bsdf->base,
                                        incoming,
                                        shading_normal,
                                        outgoing,
                                        transmitted,
                                        compositor,
                                        reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   alpha_bsdf->reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

    return status;
}

static
ISTATUS
AlphaBsdfComputeDiffuseWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    )
{
    PCALPHA_BSDF alpha_bsdf = (PCALPHA_BSDF)context;

    if (alpha_bsdf->alpha == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = BsdfComputeDiffuseWithPdf(alpha_bsdf->base,
                                               incoming,
                                               shading_normal,
                                               outgoing,
                                               transmitted,
                                               compositor,
                                               reflector,
                                               pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf *= alpha_bsdf->alpha;

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   alpha_bsdf->reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

    return ISTATUS_SUCCESS;
}

//
// Alpha BSDF Static Variables
//

static const BSDF_VTABLE alpha_bsdf_vtable = {
    AlphaBsdfSample,
    AlphaBsdfSampleDiffuse,
    AlphaBsdfComputeDiffuse,
    AlphaBsdfComputeDiffuseWithPdf,
    NULL
};

//
// Alpha BSDF Static Functions
//

static
inline
ISTATUS
AlphaBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCBSDF base_bsdf,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t alpha,
    _Out_ PCBSDF *bsdf
    )
{
    assert(bsdf_allocator != NULL);
    assert(base_bsdf != NULL);
    assert((float_t)0.0 <= alpha && alpha < (float_t)1.0);
    assert(bsdf != NULL);

    ALPHA_BSDF alpha_bsdf;
    alpha_bsdf.base = base_bsdf;
    alpha_bsdf.reflector = reflector;
    alpha_bsdf.alpha = alpha;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &alpha_bsdf_vtable,
                                           &alpha_bsdf,
                                           sizeof(ALPHA_BSDF),
                                           alignof(ALPHA_BSDF),
                                           bsdf);

    return status;   
}

//
// Types
//

typedef struct _ALPHA_MATERIAL {
    PMATERIAL base;
    PFLOAT_TEXTURE alpha;
    PREFLECTOR reflector;
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

    if (alpha >= (float_t)1.0)
    {
        *bsdf = base_bsdf;
        return ISTATUS_SUCCESS;
    }

    if (alpha < (float_t)0.0)
    {
        alpha = (float_t)0.0;
    }

    status = AlphaBsdfAllocateWithAllocator(bsdf_allocator,
                                            base_bsdf,
                                            alpha_material->reflector,
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
    ReflectorRelease(alpha_material->reflector);
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

    if (alpha_texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ALPHA_MATERIAL alpha_material;
    alpha_material.base = base_material;
    alpha_material.alpha = alpha_texture;

    ISTATUS status = UniformReflectorAllocate((float_t)1.0,
                                              &alpha_material.reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = MaterialAllocate(&alpha_material_vtable,
                              &alpha_material,
                              sizeof(ALPHA_MATERIAL),
                              alignof(ALPHA_MATERIAL),
                              material);

    if (status != ISTATUS_SUCCESS)
    {
        ReflectorRelease(alpha_material.reflector);
        return status;
    }

    MaterialRetain(base_material);
    FloatTextureRetain(alpha_texture);

    return ISTATUS_SUCCESS;
}