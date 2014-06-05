/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_vector.h

Abstract:

    This file contains the internal definitions for the vector type.

--*/

#include <irisp.h>

#ifndef _VECTOR_IRIS_INTERNAL_
#define _VECTOR_IRIS_INTERNAL_

#ifdef _IRIS_EXPORT_VECTOR_ROUTINES_
#define VectorMatrixMultiply(Matrix, Vector, Product) \
        StaticVectorMatrixMultiply(Matrix, Vector, Product)

#define VectorMatrixTransposedMultiply(Matrix, Vector, Product) \
        StaticVectorMatrixTransposedMultiply(Matrix, Vector, Product)
#endif

SFORCEINLINE
VOID
VectorInitialize(
    _Out_ PVECTOR3 Vector,
    _In_ FLOAT X, 
    _In_ FLOAT Y, 
    _In_ FLOAT Z
    )
{
    ASSERT(Vector != NULL);

    ASSERT(IsNormalFloat(X) != FALSE);
    ASSERT(IsFiniteFloat(X) != FALSE);
    ASSERT(IsNormalFloat(Y) != FALSE);
    ASSERT(IsFiniteFloat(Y) != FALSE);
    ASSERT(IsNormalFloat(Z) != FALSE);
    ASSERT(IsFiniteFloat(Z) != FALSE);

    Vector->X = X;
    Vector->Y = Y;
    Vector->Z = Z;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
VectorMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    if (Matrix == NULL ||
        Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    X = Matrix->M[0][0] * Vector.X + 
        Matrix->M[0][1] * Vector.Y + 
        Matrix->M[0][2] * Vector.Z;

    Y = Matrix->M[1][0] * Vector.X + 
        Matrix->M[1][1] * Vector.Y + 
        Matrix->M[1][2] * Vector.Z;

    Z = Matrix->M[2][0] * Vector.X + 
        Matrix->M[2][1] * Vector.Y + 
        Matrix->M[2][2] * Vector.Z;

    VectorInitialize(Product, X, Y, Z);

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
VectorMatrixTransposedMultiply(
    _In_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    if (Matrix == NULL ||
        Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    X = Matrix->M[0][0] * Vector.X + 
        Matrix->M[1][0] * Vector.Y + 
        Matrix->M[2][0] * Vector.Z;

    Y = Matrix->M[0][1] * Vector.X + 
        Matrix->M[1][1] * Vector.Y + 
        Matrix->M[2][1] * Vector.Z;

    Z = Matrix->M[0][2] * Vector.X + 
        Matrix->M[1][2] * Vector.Y + 
        Matrix->M[2][2] * Vector.Z;

    VectorInitialize(Product, X, Y, Z);

    return ISTATUS_SUCCESS;
}

#ifdef _IRIS_EXPORT_VECTOR_ROUTINES_
#undef VectorMatrixMultiply
#undef VectorMatrixTransposedMultiply
#endif

#endif // _VECTOR_IRIS_INTERNAL_