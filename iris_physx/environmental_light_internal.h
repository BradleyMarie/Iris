/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    environmental_light_internal.h

Abstract:

    The internal routines for sampling from an environmental light.

--*/

#ifndef _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_INTERNAL_
#define _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/environmental_light_vtable.h"

//
// Types
//

struct _ENVIRONMENTAL_LIGHT {
    PCENVIRONMENTAL_LIGHT_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
EnvironmentalLightComputeEmissiveInternal(
    _In_ const struct _ENVIRONMENTAL_LIGHT *environmental_light,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(environmental_light != NULL);
    assert(VectorValidate(to_light));
    assert(compositor != NULL);
    assert(spectrum != NULL);

    ISTATUS status = 
        environmental_light->vtable->compute_emissive_routine(environmental_light->data,
                                                              to_light,
                                                              compositor,
                                                              spectrum);

    return status;
}

#endif // _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_INTERNAL_