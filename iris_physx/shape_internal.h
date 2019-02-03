/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    shape_internal.h

Abstract:

    The internal routines for sampling from a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_INTERNAL_
#define _IRIS_PHYSX_SHAPE_INTERNAL_

#include <assert.h>
#include <stdatomic.h>

#include "iris_physx/light.h"
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
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    assert(shape != NULL);
    assert(ray != NULL);
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

static
inline
ISTATUS
ShapeGetEmissiveMaterial(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCEMISSIVE_MATERIAL *emissive_material
    )
{
    assert(shape != NULL);
    assert(emissive_material != NULL);

    ISTATUS status =
        shape->vtable->get_emissive_material_routine(shape->data,
                                                     face_hit,
                                                     emissive_material);

    return status;
}

static
inline
ISTATUS
ShapeSampleFaceBySolidAngle(
    _In_ PCSHAPE shape,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _Inout_ PRANDOM rng,
    _Out_ PPOINT3 sampled_point,
    _Out_ float_t *pdf
    )
{
    assert(shape != NULL);
    assert(PointValidate(hit_point));
    assert(rng != NULL);
    assert(sampled_point != NULL);
    assert(pdf != NULL);

    ISTATUS status =
        shape->vtable->sample_face_by_solid_angle_routine(shape->data,
                                                          hit_point,
                                                          face_hit,
                                                          rng,
                                                          sampled_point,
                                                          pdf);

    return status;
}

static
inline
ISTATUS
ShapeComputePdfBySolidAngle(
    _In_ PCSHAPE shape,
    _In_ PCRAY to_shape,
    _In_ float_t distance_squared,
    _In_ uint32_t face_hit,
    _Out_ float_t *pdf
    )
{
    assert(shape != NULL);
    assert(to_shape);
    assert(isfinite(distance_squared));
    assert((float_t)0.0 < distance_squared);
    assert(RayValidate(*to_shape));
    assert(pdf != NULL);

    ISTATUS status =
        shape->vtable->compute_pdf_by_solid_angle_routine(shape->data,
                                                          to_shape,
                                                          distance_squared,
                                                          face_hit,
                                                          pdf);

    return status;
}

#endif // _IRIS_PHYSX_SHAPE_INTERNAL_