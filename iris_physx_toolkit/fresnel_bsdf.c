/*++

Copyright (c) 2021 Brad Weinberger

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

typedef struct _SPECULAR_DIELECTRIC {
    PREFLECTOR reflected;
    PREFLECTOR transmitted;
    float_t refractive_index_incident;
    float_t refractive_index_transmitted;
    float_t refractive_ratio;
    float_t refractive_ratio_squared;
} SPECULAR_DIELECTRIC, *PSPECULAR_DIELECTRIC;

typedef const SPECULAR_DIELECTRIC *PCSPECULAR_DIELECTRIC;

//
// Static Functions
//

static
ISTATUS
SpecularDielectricBsdfSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCSPECULAR_DIELECTRIC specular_dielectric = (PCSPECULAR_DIELECTRIC)context;

    float_t cos_theta_incident = -VectorDotProduct(incoming, normal);
    float_t sin_squared_theta_incident =
        IMax((float_t)0.0, (float_t)1.0 - cos_theta_incident * cos_theta_incident);

    float_t sin_squared_theta_transmitted =
        specular_dielectric->refractive_ratio_squared *
        sin_squared_theta_incident;

    if ((float_t)1.0 <= sin_squared_theta_transmitted)
    {
        *reflector = specular_dielectric->reflected;
        *transmitted = false;
        *outgoing = VectorReflect(incoming, normal);
        *pdf = INFINITY;
        return ISTATUS_SUCCESS;
    }

    float_t cos_theta_transmitted =
        sqrt((float_t)1.0 - sin_squared_theta_transmitted);

    float_t product0 = specular_dielectric->refractive_index_transmitted * cos_theta_incident;
    float_t product1 = specular_dielectric->refractive_index_incident * cos_theta_transmitted;
    float_t parallel = (product0 - product1) / (product0 + product1);

    float_t product2 = specular_dielectric->refractive_index_incident * cos_theta_incident;
    float_t product3 = specular_dielectric->refractive_index_transmitted * cos_theta_transmitted;
    float_t perpendicular = (product2 - product3) / (product2 + product3);

    float_t proportion_reflected =
        (parallel * parallel + perpendicular * perpendicular) * (float_t)0.5;

    float_t random_value;
    ISTATUS status = RandomGenerateFloat(rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &random_value);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (random_value < proportion_reflected)
    {
        *reflector = specular_dielectric->reflected;
        *transmitted = false;
        *outgoing = VectorReflect(incoming, normal);
        *pdf = INFINITY;
        return ISTATUS_SUCCESS;
    }

    *reflector = specular_dielectric->transmitted;
    *transmitted = true;

    incoming = VectorScale(incoming, specular_dielectric->refractive_ratio);

    float_t coeff = specular_dielectric->refractive_ratio * cos_theta_incident -
                    cos_theta_transmitted;
    normal = VectorScale(normal, coeff);

    *outgoing = VectorAdd(incoming, normal);
    *pdf = INFINITY;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpecularDielectricBsdfComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    *reflector = NULL;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpecularDielectricBsdfComputeReflectanceWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
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
SpecularDielectricBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PSPECULAR_DIELECTRIC specular_dielectric = (PSPECULAR_DIELECTRIC)context;

    ReflectorRelease(specular_dielectric->reflected);
    ReflectorRelease(specular_dielectric->transmitted);
}

//
// Static Variables
//

static const BSDF_VTABLE perfect_specular_reflector_vtable = {
    SpecularDielectricBsdfSample,
    SpecularDielectricBsdfComputeReflectance,
    SpecularDielectricBsdfComputeReflectanceWithPdf,
    SpecularDielectricBsdfFree
};

//
// Functions
//

ISTATUS
SpecularDielectricBsdfAllocate(
    _In_opt_ PREFLECTOR reflected,
    _In_opt_ PREFLECTOR transmitted,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PBSDF *bsdf
    )
{
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

    if (reflected == NULL && transmitted == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t ratio = refractive_index_incident / refractive_index_transmitted;

    SPECULAR_DIELECTRIC specular_dielectric;
    specular_dielectric.reflected = reflected;
    specular_dielectric.transmitted = transmitted;
    specular_dielectric.refractive_index_incident = refractive_index_incident;
    specular_dielectric.refractive_index_transmitted = refractive_index_transmitted;
    specular_dielectric.refractive_ratio = ratio;
    specular_dielectric.refractive_ratio_squared = ratio * ratio;

    ISTATUS status = BsdfAllocate(&perfect_specular_reflector_vtable,
                                  &specular_dielectric,
                                  sizeof(SPECULAR_DIELECTRIC),
                                  alignof(SPECULAR_DIELECTRIC),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflected);
    ReflectorRetain(transmitted);

    return ISTATUS_SUCCESS;
}

ISTATUS
SpecularDielectricBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflected,
    _In_opt_ PCREFLECTOR transmitted,
    _In_ float_t refractive_index_incident,
    _In_ float_t refractive_index_transmitted,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(refractive_index_incident) ||
        refractive_index_incident < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(refractive_index_transmitted) ||
        refractive_index_transmitted < (float_t)1.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflected == NULL && transmitted == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t ratio = refractive_index_incident / refractive_index_transmitted;

    SPECULAR_DIELECTRIC specular_dielectric;
    specular_dielectric.reflected = (PREFLECTOR)reflected;
    specular_dielectric.transmitted = (PREFLECTOR)transmitted;
    specular_dielectric.refractive_index_incident = refractive_index_incident;
    specular_dielectric.refractive_index_transmitted = refractive_index_transmitted;
    specular_dielectric.refractive_ratio = ratio;
    specular_dielectric.refractive_ratio_squared = ratio * ratio;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &perfect_specular_reflector_vtable,
                                           &specular_dielectric,
                                           sizeof(SPECULAR_DIELECTRIC),
                                           alignof(SPECULAR_DIELECTRIC),
                                           bsdf);

    return status;   
}