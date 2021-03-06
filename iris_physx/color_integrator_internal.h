/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    color_integrator_internal.h

Abstract:

    The internal routines for a color integrator.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/color_integrator_vtable.h"

//
// Types
//

struct _COLOR_INTEGRATOR {
    PCCOLOR_INTEGRATOR_VTABLE vtable;
    void *data;
    atomic_uintptr_t reference_count;
};

//
// Static Functions
//

static
inline
ISTATUS
ColorIntegratorComputeSpectrumColorStatic(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (spectrum == NULL)
    {
        *color = ColorCreateBlack();
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->vtable->compute_spectrum_color_routine(
            color_integrator->data, spectrum, color);

    return status;
}

static
inline
ISTATUS
ColorIntegratorComputeReflectorColorStatic(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (reflector == NULL)
    {
        *color = ColorCreateBlack();
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->vtable->compute_reflector_color_routine(
            color_integrator->data, reflector, color);

    return status;
}

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_