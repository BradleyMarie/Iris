/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_integrator.c

Abstract:

    Interface representing a color matcher.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/color_integrator.h"
#include "iris_physx/color_integrator_internal.h"

//
// Functions
//

ISTATUS
ColorIntegratorAllocate(
    _In_ PCCOLOR_INTEGRATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
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

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_INTEGRATOR),
                                          alignof(COLOR_INTEGRATOR),
                                          (void **)color_integrator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_integrator)->vtable = vtable;
    (*color_integrator)->data = data_allocation;
    (*color_integrator)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}



ISTATUS
ColorIntegratorComputeSpectrumColor(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = ColorIntegratorComputeSpectrumColorStatic(color_integrator,
                                                               spectrum,
                                                               color);

    return status;
}

ISTATUS
ColorIntegratorComputeReflectorColor(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status =
        ColorIntegratorComputeReflectorColorStatic(color_integrator,
                                                   reflector,
                                                   color);

    return status;
}

void
ColorIntegratorRetain(
    _In_opt_ PCOLOR_INTEGRATOR color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return;
    }

    atomic_fetch_add(&color_integrator->reference_count, 1);
}

void
ColorIntegratorRelease(
    _In_opt_ _Post_invalid_ PCOLOR_INTEGRATOR color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&color_integrator->reference_count, 1) == 1)
    {
        if (color_integrator->vtable->free_routine)
        {
            color_integrator->vtable->free_routine(color_integrator->data);
        }

        free(color_integrator);
    }
}