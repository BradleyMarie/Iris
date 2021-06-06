/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    microfacet_distribution.h

Abstract:

    Defines an abstract microfacet distribution.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_MICROFACET_DISTRIBUTION_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_MICROFACET_DISTRIBUTION_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

struct _MICROFACET_DISTRIBUTION;

typedef
float_t
(*PMICROFACET_DISTRIBUTION_COMPUTE_D_ROUTINE)(
    _In_ const struct _MICROFACET_DISTRIBUTION* distribution,
    _In_ VECTOR3 half_angle,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    );

typedef
float_t
(*PMICROFACET_DISTRIBUTION_COMPUTE_G_ROUTINE)(
    _In_ const struct _MICROFACET_DISTRIBUTION* distribution,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 outgoing,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    );

typedef
float_t
(*PMICROFACET_DISTRIBUTION_COMPUTE_LAMBDA_ROUTINE)(
    _In_ const struct _MICROFACET_DISTRIBUTION* distribution,
    _In_ VECTOR3 vector,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    );

typedef
VECTOR3
(*PMICROFACET_DISTRIBUTION_SAMPLE_HALF_ANGLE_ROUTINE)(
    _In_ const struct _MICROFACET_DISTRIBUTION* distribution,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right,
    _In_ float_t u,
    _In_ float_t v
    );

typedef struct _MICROFACET_DISTRIBUTION_VTABLE {
    PMICROFACET_DISTRIBUTION_COMPUTE_D_ROUTINE compute_d_routine;
    PMICROFACET_DISTRIBUTION_COMPUTE_G_ROUTINE compute_g_routine;
    PMICROFACET_DISTRIBUTION_COMPUTE_LAMBDA_ROUTINE compute_lambda_routine;
    PMICROFACET_DISTRIBUTION_SAMPLE_HALF_ANGLE_ROUTINE sample_half_angle_routine;
} MICROFACET_DISTRIBUTION_VTABLE, *PMICROFACET_DISTRIBUTION_VTABLE;

typedef const MICROFACET_DISTRIBUTION_VTABLE *PCMICROFACET_DISTRIBUTION_VTABLE;

typedef struct _MICROFACET_DISTRIBUTION {
    PCMICROFACET_DISTRIBUTION_VTABLE vtable;
    float_t alpha_x;
    float_t alpha_y;
} MICROFACET_DISTRIBUTION, *PMICROFACET_DISTRIBUTION;

typedef const MICROFACET_DISTRIBUTION *PCMICROFACET_DISTRIBUTION;

//
// Functions
//

static
inline
float_t
MicrofacetLambda(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 vector,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t result = distribution->vtable->compute_lambda_routine(distribution,
                                                                  vector,
                                                                  normal,
                                                                  forward,
                                                                  right);
    return result;
}

static
inline
float_t
MicrofacetG(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 outgoing,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    if (distribution->vtable->compute_g_routine != NULL)
    {
        float_t result = distribution->vtable->compute_g_routine(distribution,
                                                                 incoming,
                                                                 outgoing,
                                                                 normal,
                                                                 forward,
                                                                 right);

        return result;
    }

    float_t lambda_incoming = MicrofacetLambda(distribution,
                                               incoming,
                                               normal,
                                               forward,
                                               right);

    float_t lambda_outgoing = MicrofacetLambda(distribution,
                                               outgoing,
                                               normal,
                                               forward,
                                               right);

    return (float_t)1.0 / ((float_t)1.0 + lambda_incoming + lambda_outgoing);
}

static
inline
float_t
MicrofacetG1(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 vector,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t lambda = MicrofacetLambda(distribution,
                                      vector,
                                      normal,
                                      forward,
                                      right);

    return (float_t)1.0 / ((float_t)1.0 + lambda);
}

static
inline
float_t
MicrofacetD(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 half_angle,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t result = distribution->vtable->compute_d_routine(distribution,
                                                             half_angle,
                                                             normal,
                                                             forward,
                                                             right);

    return result;
}

static
inline
VECTOR3
MicrofacetSample(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right,
    _In_ float_t u,
    _In_ float_t v
    )
{
    VECTOR3 result =
        distribution->vtable->sample_half_angle_routine(distribution,
                                                        incoming,
                                                        normal,
                                                        forward,
                                                        right,
                                                        u,
                                                        v);

    return result;
}

static
inline
float_t
MicrofacetPdf(
    _In_ PCMICROFACET_DISTRIBUTION distribution,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 half_angle,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 forward,
    _In_ VECTOR3 right
    )
{
    float_t d = MicrofacetD(distribution,
                            half_angle,
                            normal,
                            forward,
                            right);

    float_t g1 = MicrofacetG1(distribution,
                              incoming,
                              normal,
                              forward,
                              right);

    float_t dot = fabs(VectorDotProduct(half_angle, incoming));

    float_t abs_cos_theta_i = fabs(VectorDotProduct(normal, incoming));

    return d * g1 * dot * abs_cos_theta_i;
}

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_MICROFACET_DISTRIBUTION_