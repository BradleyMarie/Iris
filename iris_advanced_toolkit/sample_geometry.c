/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_geometry.c

Abstract:

    Routines for sampling geometry randomly.

--*/

#include "iris_advanced_toolkit/sample_geometry.h"

//
// Static Function
//

static
VECTOR3
TransformVector(
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 vector
    )
{
    VECTOR3 orthogonal = VectorCreateOrthogonal(surface_normal);
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

//
// Functions
//

ISTATUS
SampleHemisphereWithCosineWeighting(
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    )
{
    if (!VectorValidate(surface_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

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
    status = RandomGenerateFloat(rng,
                                 (float_t)0.0,
                                 (float_t)iris_two_pi,
                                 &theta);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t radius = sqrt(radius_squared);
    float_t x = radius * cos(theta);
    float_t y = radius * sin(theta);

    *result = VectorCreate(x, y, sqrt((float_t)1.0 - radius_squared));
    *result = TransformVector(surface_normal, *result);

    return ISTATUS_SUCCESS;
}

ISTATUS
SampleSphereUniformly(
    _In_ float_t radius,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    )
{
    if (!isfinite(radius) || radius <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    float_t u;
    ISTATUS status = RandomGenerateFloat(rng, (float_t)0.0f, (float_t)1.0f, &u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t v;
    status = RandomGenerateFloat(rng, (float_t)0.0f, (float_t)1.0f, &v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t z = radius - (float_t)2.0 * radius * u;
    float_t r = sqrt(IMax((float_t)0.0, radius * radius - z * z));
    float_t phi = iris_two_pi * v;

    *result = VectorCreate(r * cos(phi), r * sin(phi), z);

    return ISTATUS_SUCCESS;
}