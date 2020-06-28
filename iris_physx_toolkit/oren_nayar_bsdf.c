/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    oren_nayar_bsdf.c

Abstract:

    Implements a Oren-Nayar BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/oren_nayar_bsdf.h"

//
// Types
//

typedef struct _OREN_NAYAR_BSDF {
    PREFLECTOR reflector;
    float_t a;
    float_t b;
} OREN_NAYAR_BSDF, *POREN_NAYAR_BSDF;

typedef const OREN_NAYAR_BSDF *PCOREN_NAYAR_BSDF;

//
// Static Functions
//

static
VECTOR3
CreateOrthogonalVector(
    _In_ VECTOR3 vector
    )
{
    VECTOR_AXIS shortest = VectorDiminishedAxis(vector);

    VECTOR3 unit_vector;
    switch (shortest)
    {
        case VECTOR_X_AXIS:
            unit_vector = VectorCreate((float_t)1.0,
                                       (float_t)0.0,
                                       (float_t)0.0);
            break;
        case VECTOR_Y_AXIS:
            unit_vector = VectorCreate((float_t)0.0,
                                       (float_t)1.0,
                                       (float_t)0.0);
            break;
        case VECTOR_Z_AXIS:
            unit_vector = VectorCreate((float_t)0.0,
                                       (float_t)0.0,
                                       (float_t)1.0);
            break;
    }

    VECTOR3 orthogonal = VectorCrossProduct(vector, unit_vector);

    return VectorNormalize(orthogonal, NULL, NULL);
}

static
VECTOR3
TransformVector(
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 vector
    )
{
    VECTOR3 orthogonal = CreateOrthogonalVector(surface_normal);
    VECTOR3 cross_product = VectorCrossProduct(surface_normal, orthogonal);

    float_t x = orthogonal.x * vector.x + 
                cross_product.x * vector.y +
                surface_normal.x * vector.z;

    float_t y = orthogonal.y * vector.x + 
                cross_product.y * vector.y +
                surface_normal.y * vector.z;

    float_t z = orthogonal.z * vector.x + 
                cross_product.z * vector.y +
                surface_normal.z * vector.z;

    return VectorCreate(x, y, z);
}

static const float_t two_pi = (float_t)6.28318530717958647692528676655900;
static const float_t inv_pi = (float_t)0.31830988618379067153776752674503;
static const float_t pi = (float_t)3.14159265358979323846264338327950;

static
ISTATUS
CosineSampleHemisphere(
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 random_vector
    )
{
    assert(rng != NULL);
    assert(random_vector != NULL);

    float_t radius_squared;
    ISTATUS status = RandomGenerateFloat(rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &radius_squared);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t theta;
    status = RandomGenerateFloat(rng, (float_t)0.0, (float_t)two_pi, &theta);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t radius = sqrt(radius_squared);
    float_t x = radius * cos(theta);
    float_t y = radius * sin(theta);

    VECTOR3 result = VectorCreate(x, y, sqrt((float_t)1.0 - radius_squared));
    *random_vector = TransformVector(surface_normal, result);

    return ISTATUS_SUCCESS;
}

static
inline
void
OrenNayarInitialize(
    _Out_ POREN_NAYAR_BSDF bsdf,
    _In_ PREFLECTOR reflector,
    _In_ float_t sigma
    )
{
    float_t sigma_radians = sigma * pi / (float_t)180.0;
    float_t s_sq = sigma_radians * sigma_radians;

    bsdf->reflector = reflector;
    bsdf->a = (float_t)1.0 - (s_sq / ((float_t)2.0 * (s_sq + (float_t)0.33)));
    bsdf->b = (float_t)0.45 * s_sq / (s_sq + (float_t)0.09);
}

static
inline
float_t
OrenNayarComputeReflectance(
    _In_ PCOREN_NAYAR_BSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing
    )
{
    VECTOR3 perpendicular = CreateOrthogonalVector(normal);

    float_t cosine_theta_i = VectorDotProduct(incoming, normal);
    float_t cosine_theta_o = VectorDotProduct(outgoing, normal);

    float_t sine_theta_i = (float_t)1.0 - cosine_theta_i * cosine_theta_i;
    float_t sine_theta_o = (float_t)1.0 - cosine_theta_o * cosine_theta_o;

    float_t max_cosine;
    if (sine_theta_i > (float_t)0.0001 && sine_theta_o > (float_t)0.0001)
    {
        float_t cosine_phi_i = VectorDotProduct(incoming, perpendicular);
        float_t sine_phi_i = (float_t)1.0 - cosine_phi_i * cosine_phi_i;
        float_t cosine_phi_o = VectorDotProduct(outgoing, perpendicular);
        float_t sine_phi_o = (float_t)1.0 - cosine_phi_o * cosine_phi_o;
        float_t d_cos = cosine_phi_i * cosine_phi_o + sine_phi_i * sine_phi_o;
        max_cosine = fmax((float_t)0.0, d_cos);
    }
    else
    {
        max_cosine = (float_t)0.0;
    }

    float_t abs_cosine_theta_i = fabs(cosine_theta_i);
    float_t abs_cosine_theta_o = fabs(cosine_theta_o);

    float_t sine_a, tangent_b;
    if (abs_cosine_theta_i > abs_cosine_theta_o)
    {
        sine_a = sine_theta_o;
        tangent_b = sine_theta_i / abs_cosine_theta_i;
    }
    else
    {
        sine_a = sine_theta_i;
        tangent_b = sine_theta_o / abs_cosine_theta_o;
    }

    return inv_pi * (bsdf->a + bsdf->b * max_cosine * sine_a * tangent_b);
}

static
ISTATUS
OrenNayarBsdfSample(
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
    PCOREN_NAYAR_BSDF oren_nayar_bsdf = (PCOREN_NAYAR_BSDF)context;

    ISTATUS status = CosineSampleHemisphere(normal, rng, outgoing);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t reflectance = OrenNayarComputeReflectance(oren_nayar_bsdf,
                                                      incoming,
                                                      normal,
                                                      *outgoing);

    status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              oren_nayar_bsdf->reflector,
                                              reflectance,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *transmitted = false;
    *pdf = VectorBoundedDotProduct(*outgoing, normal) * inv_pi;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
OrenNayarBsdfComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (transmitted)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    PCOREN_NAYAR_BSDF oren_nayar_bsdf = (PCOREN_NAYAR_BSDF)context;

    float_t reflectance = OrenNayarComputeReflectance(oren_nayar_bsdf,
                                                      incoming,
                                                      normal,
                                                      outgoing);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              oren_nayar_bsdf->reflector,
                                              reflectance,
                                              reflector);

    return status;
}

static
ISTATUS
OrenNayarBsdfComputeReflectanceWithPdf(
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
    if (transmitted)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    PCOREN_NAYAR_BSDF oren_nayar_bsdf = (PCOREN_NAYAR_BSDF)context;

    float_t reflectance = OrenNayarComputeReflectance(oren_nayar_bsdf,
                                                      incoming,
                                                      normal,
                                                      outgoing);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              oren_nayar_bsdf->reflector,
                                              reflectance,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = VectorBoundedDotProduct(outgoing, normal) * inv_pi;

    return ISTATUS_SUCCESS;
}

static
void
OrenNayarBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    POREN_NAYAR_BSDF oren_nayar_bsdf = (POREN_NAYAR_BSDF)context;

    ReflectorRelease(oren_nayar_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE oren_nayar_bsdf_vtable = {
    OrenNayarBsdfSample,
    OrenNayarBsdfComputeReflectance,
    OrenNayarBsdfComputeReflectanceWithPdf,
    OrenNayarBsdfFree
};

//
// Functions
//

ISTATUS
OrenNayarBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _In_ float_t sigma,
    _Out_ PBSDF *bsdf
    )
{
    if (!isfinite(sigma))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    OREN_NAYAR_BSDF oren_nayar_bsdf;
    OrenNayarInitialize(&oren_nayar_bsdf, reflector, sigma);

    ISTATUS status = BsdfAllocate(&oren_nayar_bsdf_vtable,
                                  &oren_nayar_bsdf,
                                  sizeof(OREN_NAYAR_BSDF),
                                  alignof(OREN_NAYAR_BSDF),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}

ISTATUS
OrenNayarBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t sigma,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(sigma))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    OREN_NAYAR_BSDF oren_nayar_bsdf;
    OrenNayarInitialize(&oren_nayar_bsdf, (PREFLECTOR)reflector, sigma);

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &oren_nayar_bsdf_vtable,
                                           &oren_nayar_bsdf,
                                           sizeof(OREN_NAYAR_BSDF),
                                           alignof(OREN_NAYAR_BSDF),
                                           bsdf);

    return status;   
}