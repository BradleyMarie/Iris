/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    lambertian_brdf.c

Abstract:

    Implements a Lambertian BRDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/lambertian_brdf.h"

//
// Types
//

typedef struct _LAMBERTIAN_BRDF {
    PREFLECTOR reflector;
} LAMBERTIAN_BRDF, *PLAMBERTIAN_BRDF;

typedef const LAMBERTIAN_BRDF *PCLAMBERTIAN_BRDF;

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
ISTATUS
LambertianBrdfSample(
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
    PCLAMBERTIAN_BRDF lambertian_brdf = (PCLAMBERTIAN_BRDF)context;

    ISTATUS status = CosineSampleHemisphere(normal, rng, outgoing);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_brdf->reflector,
                                              inv_pi,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = VectorBoundedDotProduct(*outgoing, normal) * inv_pi;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LambertianBrdfComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCLAMBERTIAN_BRDF lambertian_brdf = (PCLAMBERTIAN_BRDF)context;

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_brdf->reflector,
                                              inv_pi,
                                              reflector);

    return status;
}

static
ISTATUS
LambertianBrdfComputeReflectanceWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    )
{
    PCLAMBERTIAN_BRDF lambertian_brdf = (PCLAMBERTIAN_BRDF)context;

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_brdf->reflector,
                                              inv_pi,
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
LambertianBrdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PLAMBERTIAN_BRDF lambertian_brdf = (PLAMBERTIAN_BRDF)context;

    ReflectorRelease(lambertian_brdf->reflector);
}

//
// Static Variables
//

static const BRDF_VTABLE lambertian_brdf_vtable = {
    LambertianBrdfSample,
    LambertianBrdfComputeReflectance,
    LambertianBrdfComputeReflectanceWithPdf,
    LambertianBrdfFree
};

//
// Functions
//

ISTATUS
LambertianBrdfAllocate(
    _In_ PREFLECTOR reflector,
    _Out_ PBRDF *brdf
    )
{
    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    LAMBERTIAN_BRDF lambertian_brdf;
    lambertian_brdf.reflector = reflector;

    ISTATUS status = BrdfAllocate(&lambertian_brdf_vtable,
                                  &lambertian_brdf,
                                  sizeof(LAMBERTIAN_BRDF),
                                  alignof(LAMBERTIAN_BRDF),
                                  brdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}

ISTATUS
LambertianBrdfAllocateWithAllocator(
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCBRDF *brdf
    )
{
    if (brdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    LAMBERTIAN_BRDF lambertian_brdf;
    lambertian_brdf.reflector = (PREFLECTOR)reflector;

    ISTATUS status = BrdfAllocatorAllocate(brdf_allocator,
                                           &lambertian_brdf_vtable,
                                           &lambertian_brdf,
                                           sizeof(LAMBERTIAN_BRDF),
                                           alignof(LAMBERTIAN_BRDF),
                                           brdf);

    return status;   
}