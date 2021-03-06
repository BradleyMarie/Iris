/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    aggregate.c

Abstract:

    Implements a Aggregate BSDF.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/bsdfs/aggregate.h"

//
// Defines
//

#define NUM_SUPPORTED_BSDFS 8

//
// Static Asserts
//

static_assert(sizeof(size_t) == sizeof(PBSDF), "size mismatch");

//
// Types
//

_Struct_size_bytes_(2 * sizeof(size_t) + num_bsdfs * sizeof(PBSDF))
typedef struct _AGGREGATE_BSDF {
    size_t num_bsdfs;
    size_t num_diffuse;
    _Field_size_(num_bsdfs) PBSDF bsdfs[];
} AGGREGATE_BSDF, *PAGGREGATE_BSDF;

typedef const AGGREGATE_BSDF *PCAGGREGATE_BSDF;

//
// Static Functions
//

static
ISTATUS
AggregateBsdfSample(
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
    PCAGGREGATE_BSDF aggregate_bsdf = (PCAGGREGATE_BSDF)context;

    size_t sampled_index;
    ISTATUS status = RandomGenerateIndex(rng,
                                         aggregate_bsdf->num_bsdfs,
                                         &sampled_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = BsdfSample(aggregate_bsdf->bsdfs[sampled_index],
                        incoming,
                        surface_normal,
                        shading_normal,
                        rng,
                        compositor,
                        reflector,
                        type,
                        outgoing,
                        pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*pdf == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    bool below_surface =
        VectorDotProduct(surface_normal, *outgoing) < (float_t)0.0;
    bool sampled_transmission = BsdfSampleIsTransmission(*type);
    bool compute_transmission = below_surface || sampled_transmission;

    bool specular = isinf(*pdf);

    size_t matching_bsdfs = 1;
    for (size_t i = 0; i < aggregate_bsdf->num_bsdfs; i++)
    {
        if (i == sampled_index)
        {
            continue;
        }

        PCREFLECTOR bsdf_reflector;
        float_t bsdf_pdf;
        status = BsdfComputeDiffuseWithPdf(aggregate_bsdf->bsdfs[i],
                                           incoming,
                                           shading_normal,
                                           *outgoing,
                                           compute_transmission,
                                           compositor,
                                           &bsdf_reflector,
                                           &bsdf_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (bsdf_pdf <= (float_t)0.0)
        {
            continue;
        }

        if (specular)
        {
            float_t falloff = VectorPositiveDotProduct(shading_normal,
                                                       *outgoing,
                                                       sampled_transmission);

            if (falloff <= (float_t)0.0)
            {
                *reflector = NULL;
                *pdf = (float_t)0.0;
                return ISTATUS_SUCCESS;
            }

            float_t inv_falloff = (float_t)1.0 / falloff;

            status = ReflectorCompositorAttenuateReflector(compositor,
                                                           *reflector,
                                                           inv_falloff,
                                                           reflector);

            *pdf = (float_t)1.0 + bsdf_pdf;

            specular = false;
        }
        else
        {
            *pdf += bsdf_pdf;
        }

        status = ReflectorCompositorAddReflectors(compositor,
                                                  *reflector,
                                                  bsdf_reflector,
                                                  reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        matching_bsdfs += 1;
    }

    if (matching_bsdfs != 1)
    {
        *pdf /= matching_bsdfs;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AggregateBsdfSampleDiffuse(
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
    PCAGGREGATE_BSDF aggregate_bsdf = (PCAGGREGATE_BSDF)context;

    size_t sampled_index;
    ISTATUS status = RandomGenerateIndex(rng,
                                         aggregate_bsdf->num_diffuse,
                                         &sampled_index);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = BsdfSampleDiffuse(aggregate_bsdf->bsdfs[sampled_index],
                               incoming,
                               surface_normal,
                               shading_normal,
                               rng,
                               compositor,
                               reflector,
                               type,
                               outgoing,
                               pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*pdf == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    bool below_surface =
        VectorDotProduct(surface_normal, *outgoing) < (float_t)0.0;
    bool sampled_transmission = BsdfSampleIsTransmission(*type);
    bool compute_transmission = below_surface || sampled_transmission;

    size_t matching_bsdfs = 1;
    for (size_t i = 0; i < aggregate_bsdf->num_bsdfs; i++)
    {
        if (i == sampled_index)
        {
            continue;
        }

        PCREFLECTOR bsdf_reflector;
        float_t bsdf_pdf;
        status = BsdfComputeDiffuseWithPdf(aggregate_bsdf->bsdfs[i],
                                           incoming,
                                           shading_normal,
                                           *outgoing,
                                           compute_transmission,
                                           compositor,
                                           &bsdf_reflector,
                                           &bsdf_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (bsdf_pdf <= (float_t)0.0)
        {
            continue;
        }

        status = ReflectorCompositorAddReflectors(compositor,
                                                  *reflector,
                                                  bsdf_reflector,
                                                  reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *pdf += bsdf_pdf;
        matching_bsdfs += 1;
    }

    *pdf /= (float_t)matching_bsdfs;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AggregateBsdfComputeDiffuse(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    PCAGGREGATE_BSDF aggregate_bsdf = (PCAGGREGATE_BSDF)context;

    *reflector = NULL;
    for (size_t i = 0; i < aggregate_bsdf->num_bsdfs; i++)
    {
        PCREFLECTOR bsdf_reflector;
        float_t bsdf_pdf;
        ISTATUS status = BsdfComputeDiffuseWithPdf(aggregate_bsdf->bsdfs[i],
                                                   incoming,
                                                   shading_normal,
                                                   outgoing,
                                                   transmitted,
                                                   compositor,
                                                   &bsdf_reflector,
                                                   &bsdf_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (bsdf_pdf <= (float_t)0.0)
        {
            continue;
        }

        status = ReflectorCompositorAddReflectors(compositor,
                                                  *reflector,
                                                  bsdf_reflector,
                                                  reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AggregateBsdfComputeDiffuseWithPdf(
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
    PCAGGREGATE_BSDF aggregate_bsdf = (PCAGGREGATE_BSDF)context;

    size_t matching_bsdfs = 0;
    *reflector = NULL;
    *pdf = (float_t)0.0;
    for (size_t i = 0; i < aggregate_bsdf->num_bsdfs; i++)
    {
        PCREFLECTOR bsdf_reflector;
        float_t bsdf_pdf;
        ISTATUS status = BsdfComputeDiffuseWithPdf(aggregate_bsdf->bsdfs[i],
                                                   incoming,
                                                   shading_normal,
                                                   outgoing,
                                                   transmitted,
                                                   compositor,
                                                   &bsdf_reflector,
                                                   &bsdf_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (bsdf_pdf <= (float_t)0.0)
        {
            continue;
        }

        matching_bsdfs += 1;

        status = ReflectorCompositorAddReflectors(compositor,
                                                  *reflector,
                                                  bsdf_reflector,
                                                  reflector);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *pdf += bsdf_pdf;
    }

    if (matching_bsdfs > 1)
    {
        *pdf /= matching_bsdfs;
    }

    return ISTATUS_SUCCESS;
}

static
void
AggregateBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PAGGREGATE_BSDF aggregate_bsdf = (PAGGREGATE_BSDF)context;

    for (size_t i = 0; i < aggregate_bsdf->num_bsdfs; i++)
    {
        BsdfRelease(aggregate_bsdf->bsdfs[i]);
    }
}

//
// Static Variables
//

static const BSDF_VTABLE aggregate_bsdf_vtable = {
    AggregateBsdfSample,
    AggregateBsdfSampleDiffuse,
    AggregateBsdfComputeDiffuse,
    AggregateBsdfComputeDiffuseWithPdf,
    AggregateBsdfFree
};

static const BSDF_VTABLE aggregate_specular_bsdf_vtable = {
    AggregateBsdfSample,
    NULL,
    AggregateBsdfComputeDiffuse,
    AggregateBsdfComputeDiffuseWithPdf,
    AggregateBsdfFree
};

//
// Functions
//

ISTATUS
AggregateBsdfAllocate(
    _In_reads_(num_bsdfs) PBSDF *bsdfs,
    _In_ size_t num_bsdfs,
    _Out_ PBSDF *bsdf
    )
{
    if (bsdfs == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_bsdfs == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PAGGREGATE_BSDF aggregate_bsdf =
        (PAGGREGATE_BSDF)calloc(1 + num_bsdfs, sizeof(size_t));

    if (aggregate_bsdf == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t num_diffuse = 0;
    size_t inserted_bsdfs = 0;
    for (size_t i = 0; i < num_bsdfs; i++)
    {
        if (bsdfs[i] == NULL)
        {
            continue;
        }

        bool is_diffuse;
        BsdfIsDiffuse(bsdfs[i], &is_diffuse);

        if (is_diffuse)
        {
            num_diffuse += 1;
        }

        aggregate_bsdf->bsdfs[inserted_bsdfs++] = bsdfs[i];
    }

    for (size_t i = 0; i < num_bsdfs; i++)
    {
        if (bsdfs[i] == NULL)
        {
            continue;
        }

        bool is_diffuse;
        BsdfIsDiffuse(bsdfs[i], &is_diffuse);

        if (is_diffuse)
        {
            continue;
        }

        aggregate_bsdf->bsdfs[inserted_bsdfs++] = bsdfs[i];
    }

    if (inserted_bsdfs == 0)
    {
        free(aggregate_bsdf);
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    if (inserted_bsdfs == 1)
    {
        *bsdf = aggregate_bsdf->bsdfs[0];
        free(aggregate_bsdf);
        BsdfRetain(*bsdf);
        return ISTATUS_SUCCESS;
    }

    aggregate_bsdf->num_bsdfs = inserted_bsdfs;
    aggregate_bsdf->num_diffuse = num_diffuse;

    PCBSDF_VTABLE vtable;
    if (num_diffuse == 0)
    {
        vtable = &aggregate_specular_bsdf_vtable;
    }
    else
    {
        vtable = &aggregate_bsdf_vtable;
    }

    ISTATUS status = BsdfAllocate(vtable,
                                  aggregate_bsdf,
                                  sizeof(PBSDF) * (1 + inserted_bsdfs),
                                  alignof(AGGREGATE_BSDF),
                                  bsdf);

    free(aggregate_bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    for (size_t i = 0; i < num_bsdfs; i++)
    {
        BsdfRetain(bsdfs[i]);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
AggregateBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_reads_(num_bsdfs) PCBSDF *bsdfs,
    _In_ size_t num_bsdfs,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (bsdfs == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (num_bsdfs == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (NUM_SUPPORTED_BSDFS < num_bsdfs)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t temporary_space[2 + NUM_SUPPORTED_BSDFS];
    PAGGREGATE_BSDF aggregate_bsdf = (PAGGREGATE_BSDF)(void *)&temporary_space;

    size_t num_diffuse = 0;
    size_t inserted_bsdfs = 0;
    for (size_t i = 0; i < num_bsdfs; i++)
    {
        if (bsdfs[i] == NULL)
        {
            continue;
        }

        bool is_diffuse;
        BsdfIsDiffuse(bsdfs[i], &is_diffuse);

        if (is_diffuse)
        {
            num_diffuse += 1;
        }

        aggregate_bsdf->bsdfs[inserted_bsdfs++] = (PBSDF)bsdfs[i];
    }

    for (size_t i = 0; i < num_bsdfs; i++)
    {
        if (bsdfs[i] == NULL)
        {
            continue;
        }

        bool is_diffuse;
        BsdfIsDiffuse(bsdfs[i], &is_diffuse);

        if (is_diffuse)
        {
            continue;
        }

        aggregate_bsdf->bsdfs[inserted_bsdfs++] = (PBSDF)bsdfs[i];
    }

    if (inserted_bsdfs == 0)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    if (inserted_bsdfs == 1)
    {
        *bsdf = aggregate_bsdf->bsdfs[0];
        return ISTATUS_SUCCESS;
    }

    aggregate_bsdf->num_bsdfs = inserted_bsdfs;
    aggregate_bsdf->num_diffuse = num_diffuse;

    PCBSDF_VTABLE vtable;
    if (num_diffuse == 0)
    {
        vtable = &aggregate_specular_bsdf_vtable;
    }
    else
    {
        vtable = &aggregate_bsdf_vtable;
    }

    size_t struct_size = 2 * sizeof(size_t) + num_bsdfs * sizeof(PBSDF);
    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           vtable,
                                           aggregate_bsdf,
                                           struct_size,
                                           alignof(AGGREGATE_BSDF),
                                           bsdf);

    return status; 
}