/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    microfacet.c

Abstract:

    Implements the microfacet BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/bsdfs/microfacet.h"

//
// Types
//

typedef struct _MICROFACET_BSDF {
    PREFLECTOR reflector;
    MICROFACET_DISTRIBUTION microfacet_distribution;
    FRESNEL fresnel_function;
} MICROFACET_BSDF, *PMICROFACET_BSDF;

typedef const MICROFACET_BSDF *PCMICROFACET_BSDF;

//
// Static Functions
//

static
inline
void
MicrofacetBsdfCreateCoordinateSystem(
    _In_ VECTOR3 up,
    _Out_ PVECTOR3 forward,
    _Out_ PVECTOR3 right
    )
{
    *forward = VectorCreateOrthogonal(up);
    *right = VectorCrossProduct(up, *forward);
    *right = VectorNormalize(*right, NULL, NULL);
}

static
inline
VECTOR3
MicrofacetBsdfToLocal(
    _In_ VECTOR3 vector,
    _In_ VECTOR3 up,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t x = VectorDotProduct(vector, forward);
    float_t y = VectorDotProduct(vector, right);
    float_t z = VectorDotProduct(vector, up);
    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
MicrofacetBsdfToModel(
    _In_ VECTOR3 vector,
    _In_ VECTOR3 up,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t x = vector.x * forward.x +
                vector.y * right.x +
                vector.z * up.x;
    float_t y = vector.x * forward.y +
                vector.y * right.y +
                vector.z * up.y;
    float_t z = vector.x * forward.z +
                vector.y * right.z +
                vector.z * up.z;
    return VectorCreate(x, y, z);
}

static
inline
ISTATUS
MicrofacetReflectionBsdfComputeDiffuseWithPdf(
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
    PCMICROFACET_BSDF microfacet_bsdf = (PCMICROFACET_BSDF)context;

    if (transmitted)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    incoming = VectorNegate(incoming);

    VECTOR3 forward, right;
    MicrofacetBsdfCreateCoordinateSystem(shading_normal, &forward, &right);

    VECTOR3 local_incoming = MicrofacetBsdfToLocal(incoming,
                                                   shading_normal,
                                                   forward,
                                                   right);

    VECTOR3 local_outgoing = MicrofacetBsdfToLocal(outgoing,
                                                   shading_normal,
                                                   forward,
                                                   right);

    float_t cos_theta_i = local_incoming.z;
    float_t cos_theta_o = local_outgoing.z;
    if ((cos_theta_i < (float_t)0.0) != (cos_theta_o < (float_t)0.0))
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t abs_cos_theta_i = fabs(cos_theta_i);
    float_t abs_cos_theta_o = fabs(cos_theta_o);
    if (abs_cos_theta_i == (float_t)0.0 || abs_cos_theta_o == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 local_half_angle = VectorHalfAngle(local_incoming, local_outgoing);
    if (local_half_angle.x == (float_t)0.0 &&
        local_half_angle.y == (float_t)0.0 &&
        local_half_angle.z == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    local_half_angle = VectorNormalize(local_half_angle, NULL, NULL);

    float_t cos_theta_half_angle = local_half_angle.z;
    if (cos_theta_half_angle < (float_t)0.0)
    {
        cos_theta_half_angle = -cos_theta_half_angle;
    }

    ISTATUS status = FresnelCompute(&microfacet_bsdf->fresnel_function,
                                    cos_theta_half_angle,
                                    compositor,
                                    reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t d = MicrofacetD(&microfacet_bsdf->microfacet_distribution,
                            local_half_angle);

    float_t g = MicrofacetG(&microfacet_bsdf->microfacet_distribution,
                            local_incoming,
                            local_outgoing);

    float_t attenuation =
        d * g / ((float_t)4.0 * abs_cos_theta_i * abs_cos_theta_o);
    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   *reflector,
                                                   attenuation,
                                                   reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorMultiplyReflectors(compositor,
                                                   microfacet_bsdf->reflector,
                                                   *reflector,
                                                   reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = MicrofacetPdf(&microfacet_bsdf->microfacet_distribution,
                         local_incoming,
                         local_half_angle);

    *pdf /= (float_t)4.0 * VectorDotProduct(local_incoming, local_half_angle);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
MicrofacetReflectionBsdfComputeDiffuse(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    float_t pdf;
    ISTATUS status =
        MicrofacetReflectionBsdfComputeDiffuseWithPdf(context,
                                                      incoming,
                                                      shading_normal,
                                                      outgoing,
                                                      transmitted,
                                                      compositor,
                                                      reflector,
                                                      &pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (pdf <= (float_t)0.0)
    {
        *reflector = NULL;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
MicrofacetReflectionBsdfSample(
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
    PCMICROFACET_BSDF microfacet_bsdf = (PCMICROFACET_BSDF)context;

    VECTOR3 original_incoming = incoming;
    incoming = VectorNegate(incoming);

    float_t cos_theta_i = VectorDotProduct(shading_normal, incoming);
    if (cos_theta_i == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t u;
    ISTATUS status = RandomGenerateFloat(rng, (float_t)0.0, (float_t)1.0, &u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t v;
    status = RandomGenerateFloat(rng, (float_t)0.0, (float_t)1.0, &v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    VECTOR3 forward, right;
    MicrofacetBsdfCreateCoordinateSystem(shading_normal, &forward, &right);

    VECTOR3 local_incoming = MicrofacetBsdfToLocal(incoming,
                                                   shading_normal,
                                                   forward,
                                                   right);

    VECTOR3 local_half_angle =
        MicrofacetSample(&microfacet_bsdf->microfacet_distribution,
                         local_incoming,
                         u,
                         v);

    VECTOR3 half_angle = MicrofacetBsdfToModel(local_half_angle,
                                               shading_normal,
                                               forward,
                                               right);

    if (VectorDotProduct(incoming, *outgoing) < (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    *outgoing = VectorReflect(original_incoming, half_angle);

    float_t cos_theta_o = VectorDotProduct(shading_normal, *outgoing);
    if ((cos_theta_i < (float_t)0.0) != (cos_theta_o < (float_t)0.0))
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    status = MicrofacetReflectionBsdfComputeDiffuseWithPdf(context,
                                                           original_incoming,
                                                           shading_normal,
                                                           *outgoing,
                                                           false,
                                                           compositor,
                                                           reflector,
                                                           pdf);

    *type = BSDF_SAMPLE_TYPE_REFLECTION_DIFFUSE_ONLY;

    return status;
}

static
void
MicrofacetReflectionBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PMICROFACET_BSDF microfacet_bsdf = (PMICROFACET_BSDF)context;

    ReflectorRelease(microfacet_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE microfacet_bsdf_vtable = {
    MicrofacetReflectionBsdfSample,
    MicrofacetReflectionBsdfSample,
    MicrofacetReflectionBsdfComputeDiffuse,
    MicrofacetReflectionBsdfComputeDiffuseWithPdf,
    MicrofacetReflectionBsdfFree
};

//
// Functions
//

ISTATUS
MicrofacetReflectionBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ PCMICROFACET_DISTRIBUTION microfacet_distribution,
    _In_ PCFRESNEL fresnel_function,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (microfacet_distribution == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (fresnel_function == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    MICROFACET_BSDF microfacet_bsdf;
    microfacet_bsdf.reflector = (PREFLECTOR)reflector;
    microfacet_bsdf.microfacet_distribution = *microfacet_distribution;
    microfacet_bsdf.fresnel_function = *fresnel_function;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &microfacet_bsdf_vtable,
                                           &microfacet_bsdf,
                                           sizeof(MICROFACET_BSDF),
                                           alignof(MICROFACET_BSDF),
                                           bsdf);

    return status;
}