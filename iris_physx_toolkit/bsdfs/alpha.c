/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    alpha.c

Abstract:

    Creates an alpha BSDF for blending.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/bsdfs/alpha.h"

//
// BSDF Types
//

typedef struct _ALPHA_BSDF {
    PCBSDF base;
    float_t alpha;
} ALPHA_BSDF, *PALPHA_BSDF;

typedef const ALPHA_BSDF *PCALPHA_BSDF;

//
// Static Functions
//

static
ISTATUS
AlphaTransparentBsdfSample(
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
    *reflector = iris_physx_perfect_reflector;
    *type = BSDF_SAMPLE_TYPE_TRANSMISSION_CONTAINS_SPECULAR;
    *outgoing = incoming;
    *pdf = INFINITY;

    return ISTATUS_SUCCESS;
}

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
        float_t passthrough_amount = (float_t)1.0 - alpha_bsdf->alpha;
        status =
            ReflectorCompositorAttenuateReflector(compositor,
                                                  iris_physx_perfect_reflector,
                                                  passthrough_amount,
                                                  reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *type = BSDF_SAMPLE_TYPE_TRANSMISSION_CONTAINS_SPECULAR;
        *outgoing = incoming;
        *pdf = INFINITY;

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

    *pdf *= alpha_bsdf->alpha;

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   *reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

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

    *pdf *= alpha_bsdf->alpha;

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   *reflector,
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
                                                   *reflector,
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

    if (*pdf <= (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    *pdf *= alpha_bsdf->alpha;

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   *reflector,
                                                   alpha_bsdf->alpha,
                                                   reflector);

    return status;
}

//
// Static Data
//

static const BSDF_VTABLE alpha_transparent_bsdf_vtable = {
    AlphaTransparentBsdfSample,
    NULL,
    NULL,
    NULL,
    NULL
};

static const BSDF_VTABLE alpha_translucent_bsdf_vtable = {
    AlphaBsdfSample,
    AlphaBsdfSampleDiffuse,
    AlphaBsdfComputeDiffuse,
    AlphaBsdfComputeDiffuseWithPdf,
    NULL
};

//
// Functions
//

ISTATUS
AlphaBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCBSDF base,
    _In_ float_t alpha,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(alpha) || alpha < (float_t)0.0 || (float_t)1.0 < alpha)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (alpha == (float_t)1.0)
    {
        *bsdf = base;
        return ISTATUS_SUCCESS;
    }

    if (alpha == (float_t)0.0)
    {
        ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                               &alpha_transparent_bsdf_vtable,
                                               NULL,
                                               0,
                                               0,
                                               bsdf);

        return status;
    }

    ALPHA_BSDF alpha_bsdf;
    alpha_bsdf.alpha = alpha;
    alpha_bsdf.base = base;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &alpha_translucent_bsdf_vtable,
                                           &alpha_bsdf,
                                           sizeof(ALPHA_BSDF),
                                           alignof(ALPHA_BSDF),
                                           bsdf);

    return status;
}