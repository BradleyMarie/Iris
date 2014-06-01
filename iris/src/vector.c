/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    vector.c

Abstract:

    This file contains the definitions for the vector type.

--*/

#define _IRIS_EXPORT_VECTOR_ROUTINES_
#include <irisp.h>

VOID
VectorMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    StaticVectorMatrixMultiply(Matrix, Vector, Product);
}

VOID
VectorMatrixTransposedMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    StaticVectorMatrixTransposedMultiply(Matrix, Vector, Product);
}