/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    vector.c

Abstract:

    This file contains the definitions for the vector type.

--*/

#define _IRIS_EXPORT_VECTOR_ROUTINES_
#include <irisp.h>

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VectorMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    return StaticVectorMatrixMultiply(Matrix, Vector, Product);
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VectorMatrixTransposedMultiply(
    _In_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    return StaticVectorMatrixTransposedMultiply(Matrix, Vector, Product);
}