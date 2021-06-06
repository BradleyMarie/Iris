/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    fresnel.c

Abstract:

    Initializes a dielectric fresnel function.

--*/

#include "iris_physx_toolkit/bsdfs/fresnels/dielectric.h"

//
// Static Functions
//

static
inline
float_t
FresnelDielectricCoefficient(
    _In_ float_t cos_theta_i,
    _In_ float_t eta_i,
    _In_ float_t eta_t
    )
{
    assert((float_t)1.0 <= cos_theta_i && cos_theta_i <= (float_t)1.0);
    assert(isfinite(eta_i) && (float_t)0.0 < eta_i);
    assert(isfinite(eta_t) && (float_t)0.0 < eta_t);

    if (cos_theta_i > (float_t)0.0)
    {
        float_t tmp = eta_i;
        eta_i = eta_t;
        eta_t = tmp;
        cos_theta_i = -cos_theta_i;
    }

    float_t sin_theta_i = sqrt((float_t)1.0 - cos_theta_i * cos_theta_i);
    float_t sin_theta_t = eta_i / eta_t * sin_theta_i;

    if (sin_theta_t == (float_t)1.0)
    {
        return (float_t)1.0;
    }

    float_t cos_theta_t = sqrt((float_t)1.0 - sin_theta_t * sin_theta_t);

    float_t r_parallel =
        ((eta_t * cos_theta_i) - (eta_i * cos_theta_t)) /
        ((eta_t * cos_theta_i) + (eta_i * cos_theta_t));
    float_t r_parallel2 = r_parallel * r_parallel;

    float_t r_perpendicular =
        ((eta_i * cos_theta_i) - (eta_t * cos_theta_t)) /
        ((eta_i * cos_theta_i) + (eta_t * cos_theta_t));
    float_t r_perpendicular2 = r_perpendicular * r_perpendicular;

    return (float_t)0.5 * (r_parallel2 + r_perpendicular2);
}

ISTATUS
FresnelDielectricCompute(
    _In_ PCFRESNEL function,
    _In_ float_t cosine_theta_incoming,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    float_t coefficient = FresnelDielectricCoefficient(cosine_theta_incoming,
                                                       function->eta_i,
                                                       function->eta_t);

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              iris_physx_perfect_reflector,
                                              coefficient,
                                              reflector);

    return status;
}

//
// Static Data
//

static const FRESNEL_VTABLE dielectric_fresnel_vtable = {
    FresnelDielectricCompute
};

//
// Functions
//

ISTATUS
FresnelDielectricInitialize(
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PFRESNEL function
    )
{
    if (!isfinite(eta_i) || eta_i <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    if (!isfinite(eta_t) || eta_t <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (function == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    function->vtable = &dielectric_fresnel_vtable;
    function->eta_i = eta_i;
    function->eta_t = eta_t;

    return ISTATUS_SUCCESS;
}