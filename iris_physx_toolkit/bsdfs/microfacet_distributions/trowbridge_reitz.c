/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    trowbridge_reitz.c

Abstract:

    Initializes a Trowbridge-Reitz microfacet distribution.

--*/

#include "iris_physx_toolkit/bsdfs/microfacet_distributions/microfacet_distribution.h"

//
// Static Functions
//

static
inline
void
TrowbridgeReitzSample11(
    _In_ float_t cos_theta,
    _In_ float_t u,
    _In_ float_t v,
    _Out_ float_t *slope_x,
    _Out_ float_t *slope_y
    )
{
    assert((float_t)0.0 <= cos_theta && cos_theta <= (float_t)1.0);
    assert((float_t)0.0 <= u && u <= (float_t)1.0);
    assert((float_t)0.0 <= v && v <= (float_t)1.0);
    assert(slope_x != NULL);
    assert(slope_y != NULL);

    if (cos_theta > (float_t)0.9999)
    {
        float_t r = sqrt(u / ((float_t)1.0 - u));
        float_t phi = iris_two_pi * v;
        SinCos(phi, slope_y, slope_x);
        *slope_x *= r;
        *slope_y *= r;
        return;
    }

    float_t cos2_theta = cos_theta * cos_theta;
    float_t sin2_theta = (float_t)1.0 - cos2_theta;
    float_t tan2_theta = sin2_theta / cos2_theta;
    float_t tan_theta = sqrt(tan2_theta);

    float_t inv_tan2_theta = (float_t)1.0 / tan2_theta;
    float_t g1 = (float_t)2.0 /
        ((float_t)1.0 + sqrt((float_t)1.0 + (float_t)1.0 / inv_tan2_theta));

    float_t a = (float_t)2.0 * u / g1 - (float_t)1.0;
    float_t tmp = (float_t)1.0 / (a * a - (float_t)1.0);
    tmp = IMin(tmp, (float_t)1e10);
    float_t b = tan_theta;
    float_t d = sqrt(
        IMax(b * b * tmp * tmp - (a * a - b * b) * tmp, (float_t)0.0));
    float_t slope_x_1 = b * tmp - d;
    float_t slope_x_2 = b * tmp + d;

    if (a < (float_t)0.0 || slope_x_2 > (float_t)1.0 / tan_theta)
    {
        *slope_x = slope_x_1;
    }
    else
    {
        *slope_x = slope_x_2;
    }

    assert(isfinite(*slope_x));

    float_t s;
    if (v > (float_t)0.5)
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
    assert(isfinite(*slope_y));
}

float_t
TrowbridgeReitzComputeD(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 half_angle
    )
{
    float_t cos_theta = half_angle.z;
    float_t cos2_theta = cos_theta * cos_theta;
    float_t sin2_theta = (float_t)1.0 - cos2_theta;
    float_t tan2_theta = sin2_theta / cos2_theta;

    if (isinf(tan2_theta))
    {
        return (float_t)0.0;
    }

    float_t sin2_phi, cos2_phi;
    float_t sin_theta = sqrt(sin2_theta);
    if (sin_theta == (float_t)0.0)
    {
        sin2_phi = (float_t)0.0;
        cos2_phi = (float_t)1.0;
    }
    else
    {
        float_t cos_phi =
            IMax((float_t)-1.0, IMin(half_angle.x / sin_theta, (float_t)1.0));
        float_t sin_phi =
            IMax((float_t)-1.0, IMin(half_angle.y / sin_theta, (float_t)1.0));

        sin2_phi = sin_phi * sin_phi;
        cos2_phi = cos_phi * cos_phi;
    }

    float_t e =
        (cos2_phi / (distribution->alpha_x * distribution->alpha_x) +
         sin2_phi / (distribution->alpha_y * distribution->alpha_y)) *
        tan2_theta;

    float_t result = (float_t)1.0 /
        (iris_pi * distribution->alpha_x * distribution->alpha_y *
         cos2_theta * cos2_theta * ((float_t)1.0 + e) * ((float_t)1.0 + e));

    return result;
}

float_t
TrowbridgeReitzComputeLambda(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 vector
    )
{
    float_t cos_theta = vector.z;
    float_t cos2_theta = cos_theta * cos_theta;
    float_t sin2_theta = (float_t)1.0 - cos2_theta;
    float_t tan2_theta = sin2_theta / cos2_theta;

    if (isinf(tan2_theta))
    {
        return (float_t)0.0;
    }

    float_t sin2_phi, cos2_phi;
    float_t sin_theta = sqrt(sin2_theta);
    if (sin_theta == (float_t)0.0)
    {
        sin2_phi = (float_t)0.0;
        cos2_phi = (float_t)1.0;
    }
    else
    {
        float_t cos_phi =
            IMax((float_t)-1.0, IMin(vector.x / sin_theta, (float_t)1.0));
        float_t sin_phi =
            IMax((float_t)-1.0, IMin(vector.y / sin_theta, (float_t)1.0));

        sin2_phi = sin_phi * sin_phi;
        cos2_phi = cos_phi * cos_phi;
    }

    float_t alpha2 =
        cos2_phi * distribution->alpha_x * distribution->alpha_x +
        sin2_phi * distribution->alpha_y * distribution->alpha_y;

    float_t result =
        (float_t)-1.0 + sqrt((float_t)1.0 + alpha2 * tan2_theta);
    result *= (float_t)0.5;

    return result;
}

VECTOR3
TrowbridgeReitzSampleHalfAngle(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 incoming,
    _In_ float_t u,
    _In_ float_t v
    )
{
    bool flip;
    if (incoming.z < (float_t)0.0)
    {
        incoming = VectorNegate(incoming);
        flip = true;
    }
    else
    {
        flip = false;
    }

    VECTOR3 stretched = incoming;
    stretched.x *= distribution->alpha_x;
    stretched.y *= distribution->alpha_y;
    stretched = VectorNormalize(stretched, NULL, NULL);

    float_t slope_x, slope_y;
    float_t cos_stretched_theta = stretched.z;
    TrowbridgeReitzSample11(cos_stretched_theta, u, v, &slope_x, &slope_y);

    float_t cos2_stretched_theta = cos_stretched_theta * cos_stretched_theta;
    float_t sin2_stretched_theta = (float_t)1.0 - cos2_stretched_theta;
    float_t sin_stretched_theta = sqrt(sin2_stretched_theta);

    float_t sin_stretched_phi, cos_stretched_phi;
    if (sin_stretched_theta == (float_t)0.0)
    {
        sin_stretched_phi = (float_t)0.0;
        cos_stretched_phi = (float_t)1.0;
    }
    else
    {
        cos_stretched_phi = stretched.x / sin_stretched_theta;
        cos_stretched_phi =
            IMax((float_t)-1.0, IMin(cos_stretched_phi, (float_t)1.0));

        sin_stretched_phi = stretched.y / sin_stretched_theta;
        sin_stretched_phi =
            IMax((float_t)-1.0, IMin(sin_stretched_phi, (float_t)1.0));
    }

    float_t tmp = cos_stretched_phi * slope_x - sin_stretched_phi * slope_y;
    slope_y = sin_stretched_phi * slope_x + cos_stretched_phi * slope_y;
    slope_x = tmp;

    slope_x = distribution->alpha_x * slope_x;
    slope_y = distribution->alpha_y * slope_y;

    VECTOR3 result = VectorCreate(-slope_x, -slope_y, (float_t)1.0);
    result = VectorNormalize(result, NULL, NULL);

    if (flip)
    {
        result = VectorNegate(result);
    }

    return result;
}

//
// Static Data
//

static const MICROFACET_DISTRIBUTION_VTABLE trowbridge_reitz_vtable = {
    TrowbridgeReitzComputeD,
    NULL,
    TrowbridgeReitzComputeLambda,
    TrowbridgeReitzSampleHalfAngle
};

//
// Functions
//

ISTATUS
TrowbridgeReitzInitialize(
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _Out_ PMICROFACET_DISTRIBUTION distribution
    )
{
    if (!isfinite(alpha_x))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(alpha_y))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (distribution == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    distribution->vtable = &trowbridge_reitz_vtable;
    distribution->alpha_x = IMax((float_t)0.001, alpha_x);
    distribution->alpha_y = IMax((float_t)0.001, alpha_y);

    return ISTATUS_SUCCESS;
}