/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    multiply.h

Abstract:

    The matrix-vector and matrix-point multiply routines for Iris.

--*/

#ifndef _IRIS_MULTIPLY_
#define _IRIS_MULTIPLY_

#include "iris/matrix.h"
#include "iris/ray.h"

//IRISAPI
VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

//IRISAPI
VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

//IRISAPI
VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

//IRISAPI
VECTOR3
VectorMatrixInverseTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    );

//IRISAPI
POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    );

//IRISAPI
POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    );

//IRISAPI
RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    );

//IRISAPI
RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    );

#endif // _IRIS_MULTIPLY_