/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bsdf.c

Abstract:

    Interface representing a BSDF.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/bsdf.h"
#include "iris_physx/bsdf_internal.h"

//
// Functions
//

ISTATUS
BsdfAllocate(
    _In_ PCBSDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PBSDF *bsdf
    )
{
    if (vtable == NULL || vtable->sample_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(BSDF),
                                          alignof(BSDF),
                                          (void **)bsdf,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    BsdfInitialize(vtable, data_allocation, *bsdf);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
BsdfSample(
    _In_ PCBSDF bsdf,
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
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!VectorValidate(incoming))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(surface_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!VectorValidate(shading_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (type == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    ISTATUS status = bsdf->vtable->sample_routine(bsdf->data,
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

    return ISTATUS_SUCCESS;
}

ISTATUS
BsdfSampleDiffuse(
    _In_ PCBSDF bsdf,
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
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!VectorValidate(incoming))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(surface_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!VectorValidate(shading_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (type == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    if (bsdf->vtable->sample_diffuse_routine == NULL)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = bsdf->vtable->sample_diffuse_routine(bsdf->data,
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

    if ((float_t)0.0 < *pdf &&
        (isinf(*pdf) || BsdfSampleContainsSpecular(*type)))
    {
        return ISTATUS_INVALID_RESULT;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
BsdfComputeDiffuse(
    _In_ PCBSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!VectorValidate(incoming))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(shading_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!VectorValidate(outgoing))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (bsdf->vtable->compute_diffuse_routine == NULL)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = bsdf->vtable->compute_diffuse_routine(bsdf->data,
                                                           incoming,
                                                           shading_normal,
                                                           outgoing,
                                                           transmitted,
                                                           compositor,
                                                           reflector);

    return status;
}

ISTATUS
BsdfComputeDiffuseWithPdf(
    _In_ PCBSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    )
{
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!VectorValidate(incoming))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(shading_normal))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!VectorValidate(outgoing))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (bsdf->vtable->compute_diffuse_with_pdf_routine == NULL)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = 
        bsdf->vtable->compute_diffuse_with_pdf_routine(bsdf->data,
                                                       incoming,
                                                       shading_normal,
                                                       outgoing,
                                                       transmitted,
                                                       compositor,
                                                       reflector,
                                                       pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (isinf(*pdf))
    {
        return ISTATUS_INVALID_RESULT;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
BsdfIsDiffuse(
    _In_ PCBSDF bsdf,
    _Out_ bool *is_diffuse
    )
{
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (is_diffuse == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *is_diffuse = bsdf->vtable->sample_diffuse_routine;

    return ISTATUS_SUCCESS;
}

void
BsdfRetain(
    _In_opt_ PBSDF bsdf
    )
{
    if (bsdf == NULL)
    {
        return;
    }

    atomic_fetch_add(&bsdf->reference_count, 1);
}

void
BsdfRelease(
    _In_opt_ _Post_invalid_ PBSDF bsdf
    )
{
    if (bsdf == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&bsdf->reference_count, 1) == 1)
    {
        if (bsdf->vtable->free_routine != NULL)
        {
            bsdf->vtable->free_routine(bsdf->data);
        }
    
        free(bsdf);
    }
}