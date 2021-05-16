/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    multiply.h

Abstract:

    The matrix-vector and matrix-point multiply routines for Iris.

--*/

#ifndef _IRIS_MULTIPLY_
#define _IRIS_MULTIPLY_

#include "iris/matrix.h"
#include "iris/ray.h"

VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

VECTOR3
VectorMatrixInverseTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    );

POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    );

RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    );

RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    );

#endif // _IRIS_MULTIPLY_