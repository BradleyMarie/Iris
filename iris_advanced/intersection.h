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

typedef enum _NORMAL_COORDINATE_SPACE {
    NORMAL_MODEL_COORDINATE_SPACE = 0,
    NORMAL_WORLD_COORDINATE_SPACE = 1
} NORMAL_COORDINATE_SPACE;

//
// Functions
//

static
inline
INTERSECTION
IntersectionCreate(
    _In_ RAY_DIFFERENTIAL model_ray,
    _In_ RAY_DIFFERENTIAL world_ray,
    _In_ VECTOR3 normal,
    _In_ NORMAL_COORDINATE_SPACE normal_coordinate_space,
    _In_ float_t distance
    )
{
    assert(RayDifferentialValidate(model_ray));
    assert(RayDifferentialValidate(world_ray));
    assert(model_ray.has_differentials == world_ray.has_differentials);
    assert(VectorValidate(normal));
    assert(normal_coordinate_space == NORMAL_MODEL_COORDINATE_SPACE ||
           normal_coordinate_space == NORMAL_WORLD_COORDINATE_SPACE);
    assert(isfinite(distance));

    INTERSECTION result;
    result.model_hit_point = RayEndpoint(model_ray.ray, distance);
    result.world_hit_point = RayEndpoint(world_ray.ray, distance);

    if (!model_ray.has_differentials)
    {
        result.has_derivatives = false;
        return result;
    }

    PCRAY_DIFFERENTIAL differential;
    VECTOR3 to_origin;
    if (normal_coordinate_space == NORMAL_MODEL_COORDINATE_SPACE)
    {
        differential = &model_ray;
        to_origin = VectorCreate(result.model_hit_point.x,
                                 result.model_hit_point.y,
                                 result.model_hit_point.z);
    }
    else
    {
        differential = &world_ray;
        to_origin = VectorCreate(result.world_hit_point.x,
                                 result.world_hit_point.y,
                                 result.world_hit_point.z);
    }

    distance = VectorDotProduct(normal, to_origin);

    to_origin = VectorCreate(differential->rx.origin.x,
                             differential->rx.origin.y,
                             differential->rx.origin.z);
    float_t tx = -(VectorDotProduct(normal, to_origin) - distance) /
                   VectorDotProduct(normal, differential->rx.direction);

    if (!isfinite(tx))
    {
        result.has_derivatives = false;
        return result;
    }

    to_origin = VectorCreate(differential->ry.origin.x,
                             differential->ry.origin.y,
                             differential->ry.origin.z);
    float_t ty = -(VectorDotProduct(normal, to_origin) - distance) /
                   VectorDotProduct(normal, differential->ry.direction);

    if (!isfinite(ty))
    {
        result.has_derivatives = false;
        return result;
    }

    POINT3 model_rx_hit = RayEndpoint(model_ray.rx, tx);
    result.model_dp_dx = PointSubtract(model_rx_hit, result.model_hit_point);

    POINT3 model_ry_hit = RayEndpoint(model_ray.ry, ty);
    result.model_dp_dy = PointSubtract(model_ry_hit, result.model_hit_point);

    POINT3 world_rx_hit = RayEndpoint(world_ray.rx, tx);
    result.world_dp_dx = PointSubtract(world_rx_hit, result.world_hit_point);

    POINT3 world_ry_hit = RayEndpoint(world_ray.ry, ty);
    result.world_dp_dy = PointSubtract(world_ry_hit, result.world_hit_point);

    result.has_derivatives = true;

    return result;
}

#endif // _IRIS_ADVANCED_INTERSECTION_
