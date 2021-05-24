/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    intersection.h

Abstract:

    A struct containing the model and world coordinates of an intersection as
    well as their derivatives if available.

--*/

#ifndef _IRIS_ADVANCED_INTERSECTION_
#define _IRIS_ADVANCED_INTERSECTION_

#include "iris_advanced/ray_differential.h"

//
// Types
//

typedef struct _INTERSECTION {
    POINT3 model_hit_point;
    POINT3 world_hit_point;
    VECTOR3 model_dp_dx;
    VECTOR3 model_dp_dy;
    VECTOR3 world_dp_dx;
    VECTOR3 world_dp_dy;
    bool has_derivatives;
} INTERSECTION, *PINTERSECTION;

typedef const INTERSECTION *PCINTERSECTION;

//
// Functions
//

static
inline
INTERSECTION
IntersectionCreate(
    _In_ RAY_DIFFERENTIAL world_ray,
    _In_opt_ PCMATRIX model_to_world,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point,
    _In_ VECTOR3 world_normal
    )
{
    assert(RayDifferentialValidate(world_ray));
    assert(PointValidate(model_hit_point));
    assert(PointValidate(world_hit_point));
    assert(VectorValidate(world_normal));

    INTERSECTION result;
    result.model_hit_point = model_hit_point;
    result.world_hit_point = world_hit_point;

    if (!world_ray.has_differentials)
    {
        result.has_derivatives = false;
        return result;
    }

    VECTOR3 to_origin = VectorCreate(world_hit_point.x,
                                     world_hit_point.y,
                                     world_hit_point.z);

    float_t distance = VectorDotProduct(world_normal, to_origin);

    to_origin = VectorCreate(world_ray.rx.origin.x,
                             world_ray.rx.origin.y,
                             world_ray.rx.origin.z);
    float_t tx = -(VectorDotProduct(world_normal, to_origin) - distance) /
                   VectorDotProduct(world_normal, world_ray.rx.direction);

    if (!isfinite(tx))
    {
        result.has_derivatives = false;
        return result;
    }

    to_origin = VectorCreate(world_ray.ry.origin.x,
                             world_ray.ry.origin.y,
                             world_ray.ry.origin.z);
    float_t ty = -(VectorDotProduct(world_normal, to_origin) - distance) /
                   VectorDotProduct(world_normal, world_ray.ry.direction);

    if (!isfinite(ty))
    {
        result.has_derivatives = false;
        return result;
    }

    POINT3 world_rx_hit = RayEndpoint(world_ray.rx, tx);
    result.world_dp_dx = PointSubtract(world_rx_hit, world_hit_point);

    POINT3 world_ry_hit = RayEndpoint(world_ray.ry, ty);
    result.world_dp_dy = PointSubtract(world_ry_hit, world_hit_point);

    result.model_dp_dx = VectorMatrixTransposedMultiply(model_to_world,
                                                        result.world_dp_dx);

    result.model_dp_dy = VectorMatrixTransposedMultiply(model_to_world,
                                                        result.world_dp_dy);

    result.has_derivatives = true;

    return result;
}

#endif // _IRIS_ADVANCED_INTERSECTION_
