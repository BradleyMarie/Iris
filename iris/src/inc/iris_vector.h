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
VectorMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Matrix != NULL);
    ASSERT(Vector != NULL);
    ASSERT(Product != NULL);

    X = Matrix->M[0][0] * Vector->X + 
        Matrix->M[0][1] * Vector->Y + 
        Matrix->M[0][2] * Vector->Z;

    Y = Matrix->M[1][0] * Vector->X + 
        Matrix->M[1][1] * Vector->Y + 
        Matrix->M[1][2] * Vector->Z;

    Z = Matrix->M[2][0] * Vector->X + 
        Matrix->M[2][1] * Vector->Y + 
        Matrix->M[2][2] * Vector->Z;

    VectorInitialize(Product, X, Y, Z);
}

SFORCEINLINE
VOID
VectorMatrixTransposedMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Matrix != NULL);
    ASSERT(Vector != NULL);
    ASSERT(Product != NULL);

    X = Matrix->M[0][0] * Vector->X + 
        Matrix->M[1][0] * Vector->Y + 
        Matrix->M[2][0] * Vector->Z;

    Y = Matrix->M[0][1] * Vector->X + 
        Matrix->M[1][1] * Vector->Y + 
        Matrix->M[2][1] * Vector->Z;

    Z = Matrix->M[0][2] * Vector->X + 
        Matrix->M[1][2] * Vector->Y + 
        Matrix->M[2][2] * Vector->Z;

    VectorInitialize(Product, X, Y, Z);
}

#ifdef _IRIS_EXPORT_VECTOR_ROUTINES_
#undef VectorMatrixMultiply
#undef VectorMatrixTransposedMultiply
#endif

#endif // _VECTOR_IRIS_INTERNAL_