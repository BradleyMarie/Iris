/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    lambertian_bsdf.c

Abstract:

    Implements a Lambertian BSDF.

--*/

#include <stdalign.h>

#include "iris_advanced_toolkit/sample_geometry.h"
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
ISTATUS
LambertianBsdfSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCLAMBERTIAN_BSDF lambertian_bsdf = (PCLAMBERTIAN_BSDF)context;

    ISTATUS status = SampleHemisphereWithCosineWeighting(normal, rng, outgoing);

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

    *type = BSDF_SAMPLE_TYPE_REFLECTION_DIFFUSE_ONLY;
    *pdf = VectorBoundedDotProduct(*outgoing, normal) * iris_inv_pi;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
LambertianBsdfComputeDiffuse(
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
LambertianBsdfComputeDiffuseWithPdf(
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
    LambertianBsdfSample,
    LambertianBsdfComputeDiffuse,
    LambertianBsdfComputeDiffuseWithPdf,
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