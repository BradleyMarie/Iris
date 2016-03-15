/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    vector.c

Abstract:

    This file contains the definitions for the vector type.

--*/

#define _IRIS_EXPORT_VECTOR_ROUTINES_
#include <irisp.h>

VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    return StaticVectorMatrixMultiply(Matrix, Vector);
}

VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    return StaticVectorMatrixTransposedMultiply(Matrix, Vector);
}

VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    return StaticVectorMatrixInverseMultiply(Matrix, Vector);
}

VECTOR3
VectorMatrixInverseTransposedMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    return StaticVectorMatrixInverseTransposedMultiply(Matrix, Vector);
}