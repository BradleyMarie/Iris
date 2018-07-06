/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    shape_internal.h

Abstract:

    The internal routines for sampling from a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_INTERNAL_
#define _IRIS_PHYSX_SHAPE_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/shape_vtable.h"

//
// Types
//

struct _SHAPE {
    PCSHAPE_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
ShapeTrace(
    _In_ const struct _SHAPE *shape,
    _In_ RAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    assert(shape != NULL);
    assert(RayValidate(ray));
    assert(allocator != NULL);
    assert(hit != NULL);

    ISTATUS status = shape->vtable->trace_routine(shape->data,
                                                  ray,
                                                  allocator,
                                                  hit);

    return status;
}

static
inline
ISTATUS
ShapeComputeNormal(
    _In_ const struct _SHAPE *shape,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _Out_ PVECTOR3 surface_normal
    )
{
    assert(shape != NULL);
    assert(PointValidate(hit_point));
    assert(surface_normal != NULL);

    ISTATUS status = shape->vtable->compute_normal_routine(shape->data,
                                                           hit_point,
                                                           face_hit,
                                                           surface_normal);

    return status;
}

static
inline
ISTATUS
ShapeGetMaterial(
    _In_ const struct _SHAPE *shape,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    )
{
    assert(shape != NULL);
    assert(material != NULL);

    ISTATUS status = shape->vtable->get_material_routine(shape->data,
                                                         face_hit,
                                                         material);

    return status;
}

/*
ISTATUS
ShapeGetLight(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCLIGHT *light
    );
*/

#endif // _IRIS_PHYSX_SHAPE_INTERNAL_