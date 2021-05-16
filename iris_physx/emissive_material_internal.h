/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    emissive_material_internal.h

Abstract:

    The internal routines for sampling from a emissive material.

--*/

#ifndef _IRIS_PHYSX_EMISSIVE_MATERIAL_INTERNAL_
#define _IRIS_PHYSX_EMISSIVE_MATERIAL_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/emissive_material_vtable.h"

//
// Types
//

struct _EMISSIVE_MATERIAL {
    PCEMISSIVE_MATERIAL_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
EmissiveMaterialSample(
    _In_ const struct _EMISSIVE_MATERIAL *emissive_material,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(emissive_material != NULL);
    assert(PointValidate(model_hit_point));
    assert(spectrum != NULL);

    ISTATUS status = 
        emissive_material->vtable->sample_routine(emissive_material->data,
                                                  model_hit_point,
                                                  additional_data,
                                                  spectrum);

    return status;
}

#endif // _IRIS_PHYSX_EMISSIVE_MATERIAL_INTERNAL_