/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    microfacet_bsdf.c

Abstract:

    Implements the microfacet BSDFs.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/microfacet_bsdf.h"

//
// Types
//

typedef struct _TR_DIELECTRIC {
    PREFLECTOR reflector;
    float_t alpha_x;
    float_t alpha_y;
    float_t eta_i;
    float_t eta_t;
} TR_DIELECTRIC, *PTR_DIELECTRIC;

typedef const TR_DIELECTRIC *PCTR_DIELECTRIC;

//
// Static Functions
//

static
float_t
FresnelDielectric(
    _In_ float_t cos_theta_i,
    _In_ float_t eta_i,
    _In_ float_t eta_t
    )
{
    assert((float_t)0.0 <= cos_theta_i);
    assert(isfinite(eta_i));
    assert((float_t)0.0 < eta_i);
    assert(isfinite(eta_t));
    assert((float_t)0.0 < eta_t);

    cos_theta_i = fmin(cos_theta_i, (float_t)1.0);
    float_t sin_theta_i = sqrt((float_t)1.0 - cos_theta_i * cos_theta_i);
    float_t sin_theta_t = eta_i / eta_t * sin_theta_i;

    if ((float_t)1.0 <= sin_theta_t)
    {
        return (float_t)1.0;
    }

    float_t cos_theta_t = sqrt((float_t)1.0 - sin_theta_t * sin_theta_t);

    float_t product0 = eta_t * cos_theta_i;
    float_t product1 = eta_i * cos_theta_t;
    float_t parallel = (product0 - product1) / (product0 + product1);

    float_t product2 = eta_i * cos_theta_i;
    float_t product3 = eta_t * cos_theta_t;
    float_t perpendicular = (product2 - product3) / (product2 + product3);

    return (parallel * parallel + perpendicular * perpendicular) * (float_t)0.5;
}

static
void
TrowbridgeReitzSample11(
    _In_ float_t cos_theta,
    _In_ float_t tan_theta,
    _In_ float_t u,
    _In_ float_t v,
    _Out_ float_t *slope_x,
    _Out_ float_t *slope_y
    )
{
    if (cos_theta > (float_t)0.9999) {
        float_t r = sqrt(u / ((float_t)1.0 - u));
        float_t phi = iris_two_pi * v;
        *slope_x = r * cos(phi);
        *slope_y = r * sin(phi);
        return;
    }

    float_t a = (float_t)1.0 / tan_theta;
    float_t g1 = (float_t)2.0 / ((float_t)1.0 + sqrt((float_t)1.0 + (float_t)1.0 / (a * a)));

    float_t A = 2 * u / g1 - (float_t)1.0;
    float_t tmp = fmax((float_t)1.0 / (A * A - (float_t)1.0), (float_t)1e10);

    float_t b = tan_theta;
    float_t d = sqrt(
        fmax(b * b * tmp * tmp - (a * a - b * b) * tmp, (float_t)0.0));
    float_t slope_x_1 = b * tmp - d;
    float_t slope_x_2 = b * tmp + d;

    if (A < (float_t)0.0 || slope_x_2 > (float_t)1.0 / tan_theta)
    {
        *slope_x = slope_x_1;
    }
    else
    {
        *slope_x = slope_x_2;
    }

    float_t s;
    if (v > 0.5f)
    {
        s = (float_t)1.0;
        v = (float_t)2.0 * (v - (float_t)0.5);
    }
    else
    {
        s = (float_t)-1.0;
        v = (float_t)2.0 * ((float_t)0.5 - v);
    }

    float_t z =
        (v * (v * (v * (float_t)0.27385 - (float_t)0.73369) + (float_t)0.46341)) /
        (v * (v * (v * (float_t)0.093073 + (float_t)0.309420) - (float_t)1.000000) + (float_t)0.597999);
    *slope_y = s * z * sqrt((float_t)1.0 + *slope_x * *slope_x);
}

static
inline
float_t
TrowbridgeReitzLambda(
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ VECTOR3 vector,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 orthogonal
    )
{
    float_t cos_theta, tan_theta;
    VectorCodirectionalAngleProperties(vector,
                                       normal,
                                       &cos_theta,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &tan_theta);

    if (cos_theta == (float_t)0.0)
    {
        return (float_t)0.0;
    }

    float_t cos_squared_phi, sin_squared_phi;
    VectorCodirectionalAngleProperties(vector,
                                       orthogonal,
                                       NULL,
                                       &cos_squared_phi,
                                       NULL,
                                       &sin_squared_phi,
                                       NULL);

    float_t alpha_squared =
        cos_squared_phi * alpha_x * alpha_y +
        sin_squared_phi * alpha_x * alpha_y;
    float_t alpha = sqrt(alpha_squared);

    float_t a = (float_t)1.0 / (alpha * tan_theta);

    if (a >= (float_t)1.6)
    {
        return (float_t)0.0;
    }

    float_t a_sq = a * a;
    float_t result =
        ((float_t)1.0 - (float_t)1.259 * a + (float_t)0.396 * a_sq) /
        ((float_t)3.535 * a + (float_t)2.181 * a_sq);

    return result;
}

static
inline
float_t
TrowbridgeReitzPdf(
    _In_ VECTOR3 outgoing,
    _In_ VECTOR3 half_angle,
    _In_ float_t cos_theta_o,
    _In_ float_t half_angle_cos_theta_o,
    _In_ float_t lambda_o,
    _In_ float_t d
    )
{
    assert((float_t)0.0 <= half_angle_cos_theta_o);

    float_t g1 = (float_t)1.0 / ((float_t)1.0 + lambda_o);
    return d * g1 * half_angle_cos_theta_o / cos_theta_o;
}

static
inline
float_t
TrowbridgeReitzG(
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ float_t lambda_i,
    _In_ float_t lambda_o
    )
{
    return (float_t)1.0 / ((float_t)1.0 + lambda_i + lambda_o);
}

static
inline
float_t
TrowbridgeReitzD(
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ VECTOR3 half_angle,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 orthogonal
    )
{
    float_t cos_squared_theta, tan_theta;
    VectorCodirectionalAngleProperties(half_angle,
                                       normal,
                                       NULL,
                                       &cos_squared_theta,
                                       NULL,
                                       NULL,
                                       &tan_theta);

    if (!isfinite(tan_theta))
    {
        return (float_t)0.0;
    }

    float_t tan_squared_theta = tan_theta * tan_theta;
    float_t cos_4_theta = cos_squared_theta * cos_squared_theta;

    float_t cos_squared_phi, sin_squared_phi;
    VectorCodirectionalAngleProperties(half_angle,
                                       normal,
                                       NULL,
                                       &cos_squared_phi,
                                       NULL,
                                       &sin_squared_phi,
                                       NULL);

    float_t exponent = -tan_squared_theta;
    exponent *= (cos_squared_phi / (alpha_x * alpha_x)) +
                (sin_squared_phi / (alpha_y * alpha_y));
    exponent /= iris_pi * alpha_x * alpha_y * cos_4_theta;

    return exp(exponent);
}

static
ISTATUS
TrowbridgeReitzDielectricReflectionBsdfSample(
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
    PCTR_DIELECTRIC microfacet_bsdf = (PCTR_DIELECTRIC)context;

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

    float_t cos_theta_i, tan_theta_i;
    VectorCodirectionalAngleProperties(incoming,
                                       normal,
                                       &cos_theta_i,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &tan_theta_i);

    incoming.x *= microfacet_bsdf->alpha_x;
    incoming.y *= microfacet_bsdf->alpha_y;

    incoming = VectorNormalize(incoming, NULL, NULL);

    float_t slope_x, slope_y;
    TrowbridgeReitzSample11(cos_theta_i, tan_theta_i, u, v, &slope_x, &slope_y);

    VECTOR3 orthogonal = VectorCreateOrthogonal(normal);
    VECTOR3 cross_product = VectorCrossProduct(normal, orthogonal);

    float_t cos_phi, sin_phi;
    VectorCodirectionalAngleProperties(incoming,
                                       orthogonal,
                                       &cos_phi,
                                       NULL,
                                       &sin_phi,
                                       NULL,
                                       NULL);

    float_t tmp = cos_phi * slope_x - sin_phi * slope_y;
    slope_y = sin_phi * slope_x + cos_phi * slope_y;
    slope_x = tmp;

    slope_x *= microfacet_bsdf->alpha_x;
    slope_y *= microfacet_bsdf->alpha_y;

    slope_x = -slope_x;
    slope_y = -slope_y;
    float_t slope_z = (float_t)1.0;

    float_t x = orthogonal.x * slope_x + 
                cross_product.x * slope_y +
                normal.x * slope_z;

    float_t y = orthogonal.y * slope_x + 
                cross_product.y * slope_y +
                normal.y * slope_z;

    float_t z = orthogonal.z * slope_x + 
                cross_product.z * slope_y +
                normal.z * slope_z;

    *outgoing = VectorCreate(x, y, z);
    *outgoing = VectorNormalize(*outgoing, NULL, NULL);

    float_t cos_theta_o = VectorBoundedDotProduct(*outgoing, normal);
    VECTOR3 half_angle = VectorHalfAngle(incoming, *outgoing);

    if (half_angle.x == (float_t)0.0 ||
        half_angle.y == (float_t)0.0 ||
        half_angle.z == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t lambda_o = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             *outgoing,
                                             normal,
                                             orthogonal);

    float_t microfacet_d = TrowbridgeReitzD(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            half_angle,
                                            normal,
                                            orthogonal);

    float_t half_angle_cos_theta_o = VectorBoundedDotProduct(*outgoing,
                                                             half_angle);

    *pdf = TrowbridgeReitzPdf(*outgoing,
                              half_angle,
                              cos_theta_o,
                              half_angle_cos_theta_o,
                              lambda_o,
                              microfacet_d);
    assert((float_t)0.0 < *pdf);

    float_t half_angle_cos_theta_i = VectorBoundedDotProduct(incoming,
                                                             half_angle);

    float_t lambda_i = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             incoming,
                                             normal,
                                             orthogonal);

    float_t fresnel_coeff = FresnelDielectric(half_angle_cos_theta_i,
                                              microfacet_bsdf->eta_i,
                                              microfacet_bsdf->eta_t);

    float_t microfacet_g = TrowbridgeReitzG(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            lambda_i,
                                            lambda_o);

    float_t attenuation =
        microfacet_d * microfacet_g * fresnel_coeff /
        ((float_t)4.0 * cos_theta_i * cos_theta_o);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   microfacet_bsdf->reflector,
                                                   attenuation,
                                                   reflector);

    return status;
}

static
ISTATUS
TrowbridgeReitzDielectricReflectionBsdfComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCTR_DIELECTRIC microfacet_bsdf = (PCTR_DIELECTRIC)context;

    if (transmitted)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t cos_theta_i = VectorBoundedDotProduct(incoming, normal);

    if (cos_theta_i == (float_t)0.0)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t cos_theta_o = VectorBoundedDotProduct(outgoing, normal);

    if (cos_theta_o == (float_t)0.0)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 half_angle = VectorHalfAngle(incoming, outgoing);

    if (half_angle.x == (float_t)0.0 ||
        half_angle.y == (float_t)0.0 ||
        half_angle.z == (float_t)0.0)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 orthogonal = VectorCreateOrthogonal(normal);

    float_t half_angle_cos_theta_i = VectorBoundedDotProduct(incoming,
                                                             half_angle);

    float_t lambda_i = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             incoming,
                                             normal,
                                             orthogonal);

    float_t lambda_o = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             outgoing,
                                             normal,
                                             orthogonal);

    float_t fresnel_coeff = FresnelDielectric(half_angle_cos_theta_i,
                                              microfacet_bsdf->eta_i,
                                              microfacet_bsdf->eta_t);

    float_t microfacet_d = TrowbridgeReitzD(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            half_angle,
                                            normal,
                                            orthogonal);

    float_t microfacet_g = TrowbridgeReitzG(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            lambda_i,
                                            lambda_o);

    float_t attenuation =
        microfacet_d * microfacet_g * fresnel_coeff /
        ((float_t)4.0 * cos_theta_i * cos_theta_o);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              microfacet_bsdf->reflector,
                                              attenuation,
                                              reflector);

    return status;
}

static
ISTATUS
TrowbridgeReitzDielectricReflectionBsdfComputeReflectanceWithPdf(
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
    PCTR_DIELECTRIC microfacet_bsdf = (PCTR_DIELECTRIC)context;

    if (transmitted)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t cos_theta_i = VectorBoundedDotProduct(incoming, normal);

    if (cos_theta_i == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t cos_theta_o = VectorBoundedDotProduct(outgoing, normal);

    if (cos_theta_o == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 half_angle = VectorHalfAngle(incoming, outgoing);

    if (half_angle.x == (float_t)0.0 ||
        half_angle.y == (float_t)0.0 ||
        half_angle.z == (float_t)0.0)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 orthogonal = VectorCreateOrthogonal(normal);

    float_t lambda_o = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             outgoing,
                                             normal,
                                             orthogonal);

    float_t microfacet_d = TrowbridgeReitzD(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            half_angle,
                                            normal,
                                            orthogonal);

    float_t half_angle_cos_theta_o = VectorBoundedDotProduct(outgoing,
                                                             half_angle);

    *pdf = TrowbridgeReitzPdf(outgoing,
                              half_angle,
                              cos_theta_o,
                              half_angle_cos_theta_o,
                              lambda_o,
                              microfacet_d);

    if (*pdf <= (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t half_angle_cos_theta_i = VectorBoundedDotProduct(incoming,
                                                             half_angle);

    float_t lambda_i = TrowbridgeReitzLambda(microfacet_bsdf->alpha_x,
                                             microfacet_bsdf->alpha_y,
                                             incoming,
                                             normal,
                                             orthogonal);

    float_t fresnel_coeff = FresnelDielectric(half_angle_cos_theta_i,
                                              microfacet_bsdf->eta_i,
                                              microfacet_bsdf->eta_t);

    float_t microfacet_g = TrowbridgeReitzG(microfacet_bsdf->alpha_x,
                                            microfacet_bsdf->alpha_y,
                                            lambda_i,
                                            lambda_o);

    float_t attenuation =
        microfacet_d * microfacet_g * fresnel_coeff /
        ((float_t)4.0 * cos_theta_i * cos_theta_o);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              microfacet_bsdf->reflector,
                                              attenuation,
                                              reflector);

    return status;
}

static
void
TrowbridgeReitzDielectricReflectionBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTR_DIELECTRIC microfacet_bsdf = (PTR_DIELECTRIC)context;

    ReflectorRelease(microfacet_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE microfacet_bsdf_vtable = {
    TrowbridgeReitzDielectricReflectionBsdfSample,
    TrowbridgeReitzDielectricReflectionBsdfComputeReflectance,
    TrowbridgeReitzDielectricReflectionBsdfComputeReflectanceWithPdf,
    TrowbridgeReitzDielectricReflectionBsdfFree
};

//
// Functions
//

ISTATUS
TrowbridgeReitzDielectricReflectionBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PBSDF *bsdf
    )
{
    if (!isfinite(alpha_x))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(alpha_y))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(eta_i) || eta_i <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(eta_t) || eta_t <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    TR_DIELECTRIC microfacet_bsdf;
    microfacet_bsdf.reflector = reflector;
    microfacet_bsdf.alpha_x = alpha_x;
    microfacet_bsdf.alpha_y = alpha_y;
    microfacet_bsdf.eta_i = eta_i;
    microfacet_bsdf.eta_t = eta_t;

    ISTATUS status = BsdfAllocate(&microfacet_bsdf_vtable,
                                  &microfacet_bsdf,
                                  sizeof(TR_DIELECTRIC),
                                  alignof(TR_DIELECTRIC),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return status;
}

ISTATUS
TrowbridgeReitzDielectricReflectionBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(alpha_x))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(alpha_y))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(eta_i) || eta_i <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(eta_t) || eta_t <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    TR_DIELECTRIC microfacet_bsdf;
    microfacet_bsdf.reflector = (PREFLECTOR)reflector;
    microfacet_bsdf.alpha_x = alpha_x;
    microfacet_bsdf.alpha_y = alpha_y;
    microfacet_bsdf.eta_i = eta_i;
    microfacet_bsdf.eta_t = eta_t;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &microfacet_bsdf_vtable,
                                           &microfacet_bsdf,
                                           sizeof(TR_DIELECTRIC),
                                           alignof(TR_DIELECTRIC),
                                           bsdf);
    return status;
}