/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray.h

Abstract:

    The ray type used by Iris.

--*/

#include "iris/point.h"
#include "iris/vector.h"

#ifndef _IRIS_RAY_
#define _IRIS_RAY_

//
// Types
//

typedef struct _RAY {
    POINT3 origin;
    VECTOR3 direction;
} RAY, *PRAY;

typedef const RAY *PCRAY;

//
// Functions
//

static
inline
RAY
RayCreate(
    _In_ POINT3 origin,
    _In_ VECTOR3 direction
    )
{
    RAY ray;
    ray.origin = origin;
    ray.direction = direction;

    return ray;
}

static
inline
POINT3
RayEndpoint(
    _In_ RAY ray,
    _In_ float_t distance
    )
{
    assert(isfinite(distance));

    POINT3 endpoint = PointVectorAddScaled(ray.origin,
                                           ray.direction,
                                           distance);

    return endpoint;
}

static
inline
RAY
RayNormalize(
    _In_ RAY ray
    )
{
    VECTOR3 normalized_direction = VectorNormalize(ray.direction, NULL, NULL);

    return RayCreate(ray.origin, normalized_direction);
}

static
inline
bool
RayValidate(
    _In_ RAY ray
    )
{
    return PointValidate(ray.origin) && VectorValidate(ray.direction);
}

#endif // _IRIS_RAY_