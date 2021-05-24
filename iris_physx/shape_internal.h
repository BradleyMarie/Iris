/*++

Copyright (c) 2021 Brad Weinberger

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
#include "iris_physx/normal_map_internal.h"
#include "iris_physx/shape_vtable.h"

//
// Extern Data
//

extern const NORMAL_MAP geometry_normal_map;
extern const NORMAL_MAP shading_normal_map;

//
// Types
//

struct _SHAPE {
    PCSHAPE_VTABLE vtable;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
void*
ShapeGetData(
    _In_ PCSHAPE shape
    )
{
    assert(shape != NULL);

    return (void*)((const void **)shape + 2);
}

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

    const void* data = ShapeGetData(shape);
    ISTATUS status = shape->vtable->trace_routine(data,
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

    const void* data = ShapeGetData(shape);
    ISTATUS status = shape->vtable->compute_normal_routine(data,
                                                           hit_point,
                                                           face_hit,
                                                           surface_normal);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!VectorValidate(*surface_normal))
    {
        return ISTATUS_INVALID_RESULT;
    }

    return ISTATUS_SUCCESS;
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

    const void* data = ShapeGetData(shape);
    ISTATUS status = shape->vtable->get_material_routine(data,
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

    const void* data = ShapeGetData(shape);
    ISTATUS status =
        shape->vtable->get_emissive_material_routine(data,
                                                     face_hit,
                                                     emissive_material);

    return status;
}

static
inline
ISTATUS
ShapeSampleFace(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _Inout_ PRANDOM rng,
    _Out_ PPOINT3 sampled_point
    )
{
    assert(shape != NULL);
    assert(rng != NULL);
    assert(sampled_point != NULL);

    const void* data = ShapeGetData(shape);
    ISTATUS status =
        shape->vtable->sample_face_routine(data,
                                           face_hit,
                                           rng,
                                           sampled_point);

    return status;
}

static
inline
ISTATUS
ShapeComputePdfBySolidAngle(
    _In_ PCSHAPE shape,
    _In_ PCRAY to_shape,
    _In_ float_t distance,
    _In_ uint32_t face_hit,
    _Out_ float_t *pdf
    )
{
    assert(shape != NULL);
    assert(to_shape != NULL);
    assert(isfinite(distance));
    assert((float_t)0.0 < distance);
    assert(RayValidate(*to_shape));
    assert(pdf != NULL);

    const void* data = ShapeGetData(shape);
    ISTATUS status =
        shape->vtable->compute_pdf_by_solid_angle_routine(data,
                                                          to_shape,
                                                          distance,
                                                          face_hit,
                                                          pdf);

    return status;
}

static
inline
ISTATUS
ShapeComputeShadingNormal(
    _In_ PCSHAPE shape,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 geometry_normal,
    _In_ uint32_t face_hit,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal
    )
{
    assert(shape != NULL);
    assert(PointValidate(hit_point));
    assert(VectorValidate(geometry_normal));
    assert(shading_normal != NULL);

    const void* data = ShapeGetData(shape);

    PCNORMAL_MAP normal_map;
    ISTATUS status = shape->vtable->get_normal_map_routine(data,
                                                           face_hit,
                                                           &normal_map);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (normal_map == NULL)
    {
        *shading_normal = geometry_normal;
        return ISTATUS_SUCCESS;
    }

    status = NormalMapCompute(normal_map,
                              hit_point,
                              geometry_normal,
                              additional_data,
                              texture_coordinates,
                              shading_normal);

    return status;
}

static
inline
ISTATUS
ShapeComputeTextureCoordinates(
    _In_ PCSHAPE shape,
    _In_ PCINTERSECTION intersection,
    _In_ uint32_t face_hit,
    _In_ const void *additional_data,
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _Out_ void **texture_coordinates
    )
{
    assert(shape != NULL);
    assert(intersection != NULL);
    assert(allocator != NULL);
    assert(texture_coordinates != NULL);

    if (shape->vtable->compute_texture_coordinates != NULL)
    {
        VECTOR3 dpdx_dpdy_storage[2];
        PCVECTOR3 dpdx_dpdy;
        if (intersection->has_derivatives)
        {
            dpdx_dpdy_storage[0] = intersection->model_dp_dx;
            dpdx_dpdy_storage[1] = intersection->model_dp_dy;
            dpdx_dpdy = dpdx_dpdy_storage;
        }
        else
        {
            dpdx_dpdy = NULL;
        }

        const void* data = ShapeGetData(shape);
        ISTATUS status =
            shape->vtable->compute_texture_coordinates(data,
                                                       intersection->model_hit_point,
                                                       dpdx_dpdy,
                                                       face_hit,
                                                       additional_data,
                                                       allocator,
                                                       texture_coordinates);

        return status;
    }

    *texture_coordinates = NULL;

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_PHYSX_SHAPE_INTERNAL_