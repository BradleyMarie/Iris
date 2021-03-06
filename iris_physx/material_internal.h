/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    material_internal.h

Abstract:

    The internal routines for sampling from a material.

--*/

#ifndef _IRIS_PHYSX_MATERIAL_INTERNAL_
#define _IRIS_PHYSX_MATERIAL_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/material_vtable.h"

//
// Types
//

struct _MATERIAL {
    PCMATERIAL_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
MaterialSampleInternal(
    _In_ const struct _MATERIAL *material,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    assert(material != NULL);
    assert(intersection != NULL);
    assert(bsdf_allocator != NULL);
    assert(reflector_compositor != NULL);
    assert(bsdf != NULL);

    ISTATUS status = material->vtable->sample_routine(material->data,
                                                      intersection,
                                                      additional_data,
                                                      texture_coordinates,
                                                      bsdf_allocator,
                                                      reflector_compositor,
                                                      bsdf);

    return status;
}

#endif // _IRIS_PHYSX_MATERIAL_INTERNAL_