/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    lambertian_bsdf.c

Abstract:

    Implements a Lambertian BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/lambertian_bsdf.h"

//
// Types
//

typedef struct _LAMBERTIAN_BSDF {
    PREFLECTOR reflector;
} LAMBERTIAN_BSDF, *PLAMBERTIAN_BSDF;

typedef const LAMBERTIAN_BSDF *PCLAMBERTIAN_BSDF;

//
// Static Functions
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

    VECTOR3 result = VectorCreate(x, y, sqrt((float_t)1.0 - radius_squared));
    *random_vector = TransformVector(surface_normal, result);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LambertianBsdfSample(
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
    PCLAMBERTIAN_BSDF lambertian_bsdf = (PCLAMBERTIAN_BSDF)context;

    ISTATUS status = CosineSampleHemisphere(normal, rng, outgoing);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_bsdf->reflector,
                                              iris_inv_pi,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *transmitted = false;
    *pdf = VectorBoundedDotProduct(*outgoing, normal) * iris_inv_pi;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LambertianBsdfComputeReflectance(
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

    PCLAMBERTIAN_BSDF lambertian_bsdf = (PCLAMBERTIAN_BSDF)context;

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_bsdf->reflector,
                                              iris_inv_pi,
                                              reflector);

    return status;
}

static
ISTATUS
LambertianBsdfComputeReflectanceWithPdf(
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

    PCLAMBERTIAN_BSDF lambertian_bsdf = (PCLAMBERTIAN_BSDF)context;

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              lambertian_bsdf->reflector,
                                              iris_inv_pi,
                                              reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = VectorBoundedDotProduct(outgoing, normal) * iris_inv_pi;

    return ISTATUS_SUCCESS;
}

static
void
LambertianBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PLAMBERTIAN_BSDF lambertian_bsdf = (PLAMBERTIAN_BSDF)context;

    ReflectorRelease(lambertian_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE lambertian_bsdf_vtable = {
    LambertianBsdfSample,
    LambertianBsdfComputeReflectance,
    LambertianBsdfComputeReflectanceWithPdf,
    LambertianBsdfFree
};

//
// Functions
//

ISTATUS
LambertianBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _Out_ PBSDF *bsdf
    )
{
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    LAMBERTIAN_BSDF lambertian_bsdf;
    lambertian_bsdf.reflector = reflector;

    ISTATUS status = BsdfAllocate(&lambertian_bsdf_vtable,
                                  &lambertian_bsdf,
                                  sizeof(LAMBERTIAN_BSDF),
                                  alignof(LAMBERTIAN_BSDF),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}

ISTATUS
LambertianBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
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

    LAMBERTIAN_BSDF lambertian_bsdf;
    lambertian_bsdf.reflector = (PREFLECTOR)reflector;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &lambertian_bsdf_vtable,
                                           &lambertian_bsdf,
                                           sizeof(LAMBERTIAN_BSDF),
                                           alignof(LAMBERTIAN_BSDF),
                                           bsdf);

    return status;   
}