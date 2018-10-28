/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    brdf.c

Abstract:

    Interface representing a BRDF.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/brdf.h"
#include "iris_physx/brdf_internal.h"

//
// Functions
//

ISTATUS
BrdfAllocate(
    _In_ PCBRDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PBRDF *brdf
    )
{
    if (vtable == NULL)
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

    if (brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(BRDF),
                                          alignof(BRDF),
                                          (void **)brdf,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    BrdfInitialize(vtable, data_allocation, *brdf);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
BrdfSample(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    if (brdf == NULL)
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

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    ISTATUS status = brdf->vtable->sample_routine(brdf->data,
                                                  incoming,
                                                  surface_normal,
                                                  rng,
                                                  allocator,
                                                  reflector,
                                                  outgoing,
                                                  pdf);

    // Should this be made into something stronger than an assertion?
    assert(isgreater(*pdf, (float_t)0.0));

    return status;
}

ISTATUS
BrdfComputeReflectance(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (brdf == NULL)
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

    if (!VectorValidate(outgoing))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    ISTATUS status = brdf->vtable->compute_reflectance_routine(brdf->data,
                                                               incoming,
                                                               surface_normal,
                                                               outgoing,
                                                               allocator,
                                                               reflector);

    return status;
}

ISTATUS
BrdfComputeReflectanceWithPdf(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    )
{
    if (brdf == NULL)
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

    if (!VectorValidate(outgoing))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    ISTATUS status = 
        brdf->vtable->compute_reflectance_with_pdf_routine(brdf->data,
                                                           incoming,
                                                           surface_normal,
                                                           outgoing,
                                                           allocator,
                                                           reflector,
                                                           pdf);
    
    // Should this be made into something stronger than an assertion?
    assert(isfinite(*pdf));
    assert((float_t)0.0 <= *pdf);

    return status;
}

void
BrdfRetain(
    _In_opt_ PBRDF brdf
    )
{
    if (brdf == NULL)
    {
        return;
    }

    atomic_fetch_add(&brdf->reference_count, 1);
}

void
BrdfRelease(
    _In_opt_ _Post_invalid_ PBRDF brdf
    )
{
    if (brdf == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&brdf->reference_count, 1) == 1)
    {
        if (brdf->vtable->free_routine != NULL)
        {
            brdf->vtable->free_routine(brdf->data);
        }
    
        free(brdf);
    }
}