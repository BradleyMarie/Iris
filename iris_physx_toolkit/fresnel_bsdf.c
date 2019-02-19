/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    fresnel_bsdf.c

Abstract:

    Implements a Fresnel BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/fresnel_bsdf.h"

//
// Types
//

typedef struct _PERFECT_SPECULAR_BSDF {
    PREFLECTOR reflector;
    float_t refractive_index_incident;
    float_t refractive_index_transmitted;
} PERFECT_SPECULAR_BSDF, *PPERFECT_SPECULAR_BSDF;

typedef const PERFECT_SPECULAR_BSDF *PCPERFECT_SPECULAR_BSDF;

//
// Static Functions
//

static
inline
float_t
CosineSineConvert(
    _In_ float_t value
    )
{
    return sqrt(fmax((float_t)0.0, (float_t)1.0 - value * value));
}

static
inline
float_t
FresnelDielectric(
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _In_ float_t cosine_theta_incident
    )
{
    assert((float_t)0.0 <= cosine_theta_incident);
    assert(cosine_theta_incident <= (float_t)1.0);

    float_t sin_theta_incident = CosineSineConvert(cosine_theta_incident);

    float_t ratio = refractive_index_incident / refractive_index_transmitted;
    float_t sin_theta_transmitted = ratio * sin_theta_incident;

    if ((float_t)1.0 <= sin_theta_transmitted)
    {
        return (float_t)1.0;
    }

    float_t cosine_theta_transmitted = CosineSineConvert(sin_theta_transmitted);

    float_t product0 = refractive_index_transmitted * cosine_theta_incident;
    float_t product1 = refractive_index_incident * cosine_theta_transmitted;
    float_t parallel = (product0 - product1) / (product0 + product1);

    float_t product2 = refractive_index_incident * cosine_theta_incident;
    float_t product3 = refractive_index_transmitted * cosine_theta_transmitted;
    float_t perpendicular = (product2 - product3) / (product2 + product3);

    return (parallel * parallel + perpendicular * perpendicular) * (float_t)0.5;
}

static
ISTATUS
PerfectSpecularReflectorSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCPERFECT_SPECULAR_BSDF perfect_specular_bsdf = (PCPERFECT_SPECULAR_BSDF)context;

    float_t neg_cos_theta = VectorDotProduct(incoming, normal);
    float_t cos_theta = -neg_cos_theta;

    float_t coefficient =
        FresnelDielectric(perfect_specular_bsdf->refractive_index_incident,
                          perfect_specular_bsdf->refractive_index_transmitted,
                          cos_theta);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              perfect_specular_bsdf->reflector,
                                              coefficient / cos_theta,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *outgoing = VectorReflect(incoming, normal);
    *pdf = INFINITY;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PerfectSpecularTransmitterSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    // TODO

    return ISTATUS_ALLOCATION_FAILED;
}

static
ISTATUS
PerfectSpecularComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    *reflector = NULL;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PerfectSpecularComputeReflectanceWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    )
{
    *pdf = (float_t)0.0;

    return ISTATUS_SUCCESS;
}


static
void
PerfectSpecularFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPERFECT_SPECULAR_BSDF perfect_specular_bsdf = (PPERFECT_SPECULAR_BSDF)context;

    ReflectorRelease(perfect_specular_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE perfect_specular_reflector_vtable = {
    PerfectSpecularReflectorSample,
    PerfectSpecularComputeReflectance,
    PerfectSpecularComputeReflectanceWithPdf,
    PerfectSpecularFree
};

static const BSDF_VTABLE perfect_specular_transmitter_vtable = {
    PerfectSpecularTransmitterSample,
    PerfectSpecularComputeReflectance,
    PerfectSpecularComputeReflectanceWithPdf,
    PerfectSpecularFree
};

//
// Functions
//

ISTATUS
PerfectSpecularReflectorAllocate(
    _In_ PREFLECTOR reflector,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PBSDF *bsdf
    )
{
    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(refractive_index_incident) ||
        refractive_index_incident < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(refractive_index_transmitted) ||
        refractive_index_transmitted < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    PERFECT_SPECULAR_BSDF perfect_specular_bsdf;
    perfect_specular_bsdf.reflector = reflector;
    perfect_specular_bsdf.refractive_index_incident = refractive_index_incident;
    perfect_specular_bsdf.refractive_index_transmitted = refractive_index_transmitted;

    ISTATUS status = BsdfAllocate(&perfect_specular_reflector_vtable,
                                  &perfect_specular_bsdf,
                                  sizeof(PERFECT_SPECULAR_BSDF),
                                  alignof(PERFECT_SPECULAR_BSDF),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}


ISTATUS
PerfectSpecularReflectorAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCREFLECTOR reflector,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(refractive_index_incident) ||
        refractive_index_incident < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(refractive_index_transmitted) ||
        refractive_index_transmitted < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PERFECT_SPECULAR_BSDF perfect_specular_bsdf;
    perfect_specular_bsdf.reflector = (PREFLECTOR)reflector;
    perfect_specular_bsdf.refractive_index_incident = refractive_index_incident;
    perfect_specular_bsdf.refractive_index_transmitted = refractive_index_transmitted;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &perfect_specular_reflector_vtable,
                                           &perfect_specular_bsdf,
                                           sizeof(PERFECT_SPECULAR_BSDF),
                                           alignof(PERFECT_SPECULAR_BSDF),
                                           bsdf);

    return status;   
}

ISTATUS
PerfectSpecularTransmitterAllocate(
    _In_ PREFLECTOR reflector,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PBSDF *bsdf
    )
{
    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(refractive_index_incident) ||
        refractive_index_incident < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(refractive_index_transmitted) ||
        refractive_index_transmitted < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    PERFECT_SPECULAR_BSDF perfect_specular_bsdf;
    perfect_specular_bsdf.reflector = reflector;
    perfect_specular_bsdf.refractive_index_incident = refractive_index_incident;
    perfect_specular_bsdf.refractive_index_transmitted = refractive_index_transmitted;

    ISTATUS status = BsdfAllocate(&perfect_specular_transmitter_vtable,
                                  &perfect_specular_bsdf,
                                  sizeof(PERFECT_SPECULAR_BSDF),
                                  alignof(PERFECT_SPECULAR_BSDF),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}


ISTATUS
PerfectSpecularTransmitterAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCREFLECTOR reflector,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(refractive_index_incident) ||
        refractive_index_incident < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(refractive_index_transmitted) ||
        refractive_index_transmitted < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PERFECT_SPECULAR_BSDF perfect_specular_bsdf;
    perfect_specular_bsdf.reflector = (PREFLECTOR)reflector;
    perfect_specular_bsdf.refractive_index_incident = refractive_index_incident;
    perfect_specular_bsdf.refractive_index_transmitted = refractive_index_transmitted;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &perfect_specular_transmitter_vtable,
                                           &perfect_specular_bsdf,
                                           sizeof(PERFECT_SPECULAR_BSDF),
                                           alignof(PERFECT_SPECULAR_BSDF),
                                           bsdf);

    return status;   
}