/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    multiply_internal.h

Abstract:

    The inline matrix-vector and matrix-point multiply routines for Iris.

--*/

#ifndef _IRIS_MULTIPLY_INTERNAL_
#define _IRIS_MULTIPLY_INTERNAL_

#include "iris/matrix_internal.h"
#include "iris/ray.h"

static
inline
VECTOR3
VectorMatrixMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ VECTOR3 vector
    )
{
    assert(matrix != NULL);

    float_t x = matrix->m[0][0] * vector.x + 
                matrix->m[0][1] * vector.y + 
                matrix->m[0][2] * vector.z;

    float_t y = matrix->m[1][0] * vector.x + 
                matrix->m[1][1] * vector.y + 
                matrix->m[1][2] * vector.z;

    float_t z = matrix->m[2][0] * vector.x + 
                matrix->m[2][1] * vector.y + 
                matrix->m[2][2] * vector.z;

    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
VectorMatrixTransposedMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ VECTOR3 vector
    )
{
    assert(matrix != NULL);

    float_t x = matrix->m[0][0] * vector.x + 
                matrix->m[1][0] * vector.y + 
                matrix->m[2][0] * vector.z;

    float_t y = matrix->m[0][1] * vector.x + 
                matrix->m[1][1] * vector.y + 
                matrix->m[2][1] * vector.z;

    float_t z = matrix->m[0][2] * vector.x + 
                matrix->m[1][2] * vector.y + 
                matrix->m[2][2] * vector.z;

    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
VectorMatrixInverseMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ VECTOR3 vector
    )
{
    assert(matrix != NULL);

    return VectorMatrixMultiplyInline(matrix->inverse, vector);
}

static
inline
VECTOR3
VectorMatrixInverseTransposedMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ VECTOR3 vector
    )
{
    assert(matrix != NULL);

    return VectorMatrixTransposedMultiplyInline(matrix->inverse, vector);
}

static
inline
POINT3
PointCreateScaled(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _In_ float_t w
    )
{
    assert(isfinite(x));
    assert(isfinite(y));
    assert(isfinite(z));
    assert(isfinite(w));
    assert(w != (float_t) 0.0);

    float_t scalar = (float_t) 1.0 / w;
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return PointCreate(x, y, z);
}

static
inline
POINT3
PointMatrixMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ POINT3 point
    )
{
    assert(matrix != NULL);

    float_t x = matrix->m[0][0] * point.x +
                matrix->m[0][1] * point.y +
                matrix->m[0][2] * point.z +
                matrix->m[0][3];

    float_t y = matrix->m[1][0] * point.x +
                matrix->m[1][1] * point.y +
                matrix->m[1][2] * point.z +
                matrix->m[1][3];

    float_t z = matrix->m[2][0] * point.x +
                matrix->m[2][1] * point.y +
                matrix->m[2][2] * point.z +
                matrix->m[2][3];

    float_t w = matrix->m[3][0] * point.x +
                matrix->m[3][1] * point.y +
                matrix->m[3][2] * point.z +
                matrix->m[3][3];

    return PointCreateScaled(x, y, z, w);
}

static
inline
POINT3
PointMatrixInverseMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ POINT3 point
    )
{
    assert(matrix != NULL);

    return PointMatrixMultiplyInline(matrix->inverse, point);
}

RAY
RayMatrixMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ RAY ray
    )
{
    assert(matrix != NULL);

    POINT3 transformed_origin = PointMatrixMultiplyInline(matrix, ray.origin);
    VECTOR3 transformed_direction = VectorMatrixMultiplyInline(matrix, 
                                                               ray.direction);
    
    return RayCreate(transformed_origin, transformed_direction);
}

RAY
RayMatrixInverseMultiplyInline(
    _In_ const struct _MATRIX *matrix,
    _In_ RAY ray
    )
{
    assert(matrix != NULL);

    return RayMatrixMultiplyInline(matrix->inverse, ray);
}

#endif // _IRIS_MULTIPLY_INTERNAL_