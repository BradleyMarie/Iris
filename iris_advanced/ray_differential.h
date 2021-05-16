/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    vector.h

Abstract:

    The vector type used by Iris.

--*/

#ifndef _IRIS_ADVANCED_RAY_DIFFERENTIAL_
#define _IRIS_ADVANCED_RAY_DIFFERENTIAL_

#include "iris/iris.h"

//
// Types
//

typedef struct _RAY_DIFFERENTIAL {
    RAY ray;
    RAY rx;
    RAY ry;
    bool has_differentials;
} RAY_DIFFERENTIAL, *PRAY_DIFFERENTIAL;

typedef const RAY_DIFFERENTIAL *PCRAY_DIFFERENTIAL;

//
// Functions
//

static
inline
RAY_DIFFERENTIAL
RayDifferentialCreate(
    _In_ RAY ray,
    _In_ RAY rx,
    _In_ RAY ry
    )
{
    assert(RayValidate(ray));
    assert(RayValidate(rx));
    assert(RayValidate(ry));

    RAY_DIFFERENTIAL result;
    result.ray = ray;
    result.rx = rx;
    result.ry = ry;
    result.has_differentials = true;

    return result;
}

static
inline
RAY_DIFFERENTIAL
RayDifferentialCreateWithoutDifferentials(
    _In_ RAY ray
    )
{
    assert(RayValidate(ray));

    RAY_DIFFERENTIAL result;
    result.ray = ray;
    result.rx = ray;
    result.ry = ray;
    result.has_differentials = false;

    return result;
}

static
inline
bool
RayDifferentialValidate(
    _In_ RAY_DIFFERENTIAL ray_differential
    )
{
    bool result = RayValidate(ray_differential.ray) &&
                  RayValidate(ray_differential.rx) &&
                  RayValidate(ray_differential.ry);

    return result;
}

static
inline
RAY_DIFFERENTIAL
RayDifferentialMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY_DIFFERENTIAL ray_differential
    )
{
    RAY transformed = RayMatrixMultiply(matrix, ray_differential.ray);

    if (ray_differential.has_differentials)
    {
        RAY_DIFFERENTIAL result;
        result.ray = transformed;
        result.rx = RayMatrixMultiply(matrix, ray_differential.rx);
        result.ry = RayMatrixMultiply(matrix, ray_differential.ry);
        result.has_differentials = true;

        return result;
    }

    RAY_DIFFERENTIAL result =
        RayDifferentialCreateWithoutDifferentials(transformed);

    return result;
}

static
inline
RAY_DIFFERENTIAL
RayDifferentialMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY_DIFFERENTIAL ray_differential
    )
{
    RAY transformed = RayMatrixInverseMultiply(matrix, ray_differential.ray);

    if (ray_differential.has_differentials)
    {
        RAY_DIFFERENTIAL result;
        result.ray = transformed;
        result.rx = RayMatrixInverseMultiply(matrix, ray_differential.rx);
        result.ry = RayMatrixInverseMultiply(matrix, ray_differential.ry);
        result.has_differentials = true;

        return result;
    }

    RAY_DIFFERENTIAL result =
        RayDifferentialCreateWithoutDifferentials(transformed);

    return result;
}

static
inline
RAY_DIFFERENTIAL
RayDifferentialNormalize(
    _In_ RAY_DIFFERENTIAL ray_differential
    )
{
    ray_differential.ray.direction =
        VectorNormalize(ray_differential.ray.direction, NULL, NULL);

    if (ray_differential.has_differentials)
    {
        ray_differential.rx.direction =
            VectorNormalize(ray_differential.rx.direction, NULL, NULL);
        ray_differential.ry.direction =
            VectorNormalize(ray_differential.ry.direction, NULL, NULL);

        return ray_differential;
    }

    ray_differential.rx.direction = ray_differential.ray.direction;
    ray_differential.ry.direction = ray_differential.ray.direction;

    return ray_differential;
}

#endif // _IRIS_ADVANCED_RAY_DIFFERENTIAL_