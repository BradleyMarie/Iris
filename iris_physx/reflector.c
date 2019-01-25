/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector.c

Abstract:

    Interface representing a reflector of light.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/reflector.h"
#include "iris_physx/reflector_internal.h"

//
// Functions
//

ISTATUS
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PREFLECTOR *reflector
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

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(REFLECTOR),
                                          alignof(REFLECTOR),
                                          (void **)reflector,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ReflectorInitialize(vtable,
                        data_allocation,
                        *reflector);

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorReflect(
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    if (!isfinite(wavelength) || 
        wavelength <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflectance == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        *reflectance = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = ReflectorReflectInline(reflector,
                                            wavelength,
                                            reflectance);

    // Should these be made into something stronger than assertions?
    assert((float_t)0.0 <= *reflectance);
    assert(*reflectance <= (float_t)1.0);

    return status;
}

ISTATUS
ReflectorGetAlbedo(
    _In_opt_ PCREFLECTOR reflector,
    _Out_ float_t *albedo
    )
{
    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (albedo == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = ReflectorGetAlbedoInline(reflector, albedo);

    // Should this be made into something stronger than an assertion?
    assert((float_t)0.0 <= *albedo && *albedo <= (float_t)1.0);

    return status;
}

void
ReflectorRetain(
    _In_opt_ PREFLECTOR reflector
    )
{
    if (reflector == NULL)
    {
        return;
    }

    atomic_fetch_add(&reflector->reference_count, 1);
}

void
ReflectorRelease(
    _In_opt_ _Post_invalid_ PREFLECTOR reflector
    )
{
    if (reflector == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&reflector->reference_count, 1) == 1)
    {
        if (reflector->vtable->free_routine != NULL)
        {
            reflector->vtable->free_routine(reflector->data);
        }
    
        free(reflector);
    }
}