/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    multiply.c

Abstract:

    The matrix-vector and matrix-point multiply routines for Iris.

--*/

#include "iris/multiply.h"

#include "iris/multiply_internal.h"

//IRISAPI
VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    )
{
    if (matrix == NULL) 
    {
        return vector;
    }

    return VectorMatrixMultiplyInline(matrix, vector);
}

//IRISAPI
VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    )
{
    if (matrix == NULL) 
    {
        return vector;
    }

    return VectorMatrixTransposedMultiplyInline(matrix, vector);
}

//IRISAPI
VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    )
{
    if (matrix == NULL) 
    {
        return vector;
    }

    return VectorMatrixInverseMultiplyInline(matrix, vector);
}

//IRISAPI
VECTOR3
VectorMatrixInverseTransposedMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ VECTOR3 vector
    )
{
    if (matrix == NULL) 
    {
        return vector;
    }

    return VectorMatrixInverseTransposedMultiplyInline(matrix, vector);
}

//IRISAPI
POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    )
{
    if (matrix == NULL) 
    {
        return point;
    }

    return PointMatrixMultiplyInline(matrix, point);
}

//IRISAPI
POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ POINT3 point
    )
{
    if (matrix == NULL) 
    {
        return point;
    }

    return PointMatrixInverseMultiplyInline(matrix, point);
}

//IRISAPI
RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    )
{
    if (matrix == NULL) 
    {
        return ray;
    }

    return RayMatrixMultiplyInline(matrix, ray);
}

//IRISAPI
RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX matrix,
    _In_ RAY ray
    )
{
    if (matrix == NULL) 
    {
        return ray;
    }

    return RayMatrixInverseMultiplyInline(matrix, ray);
}