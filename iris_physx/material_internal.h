/*++

Copyright (c) 2019 Brad Weinberger

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
MaterialSample(
    _In_ const struct _MATERIAL *material,
    _In_ POINT3 model_hit_point,
    _In_ VECTOR3 world_surface_normal,
    _In_ const void *additional_data,
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    )
{
    assert(material != NULL);
    assert(PointValidate(model_hit_point));
    assert(VectorValidate(world_surface_normal));
    assert(brdf_allocator != NULL);
    assert(reflector_allocator != NULL);
    assert(world_shading_normal != NULL);
    assert(brdf != NULL);

    ISTATUS status = material->vtable->sample_routine(material->data,
                                                      model_hit_point,
                                                      world_surface_normal,
                                                      additional_data,
                                                      brdf_allocator,
                                                      reflector_allocator,
                                                      world_shading_normal,
                                                      brdf);

    return status;
}

#endif // _IRIS_PHYSX_MATERIAL_INTERNAL_