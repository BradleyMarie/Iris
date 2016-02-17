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
#define VectorMatrixReferenceMultiply(Matrix, Vector) \
        StaticVectorMatrixReferenceMultiply(Matrix, Vector)

#define VectorMatrixReferenceTransposedMultiply(Matrix, Vector) \
        StaticVectorMatrixReferenceTransposedMultiply(Matrix, Vector)

#define VectorMatrixReferenceInverseMultiply(Matrix, Vector) \
        StaticVectorMatrixReferenceInverseMultiply(Matrix, Vector)

#define VectorMatrixReferenceInverseTransposedMultiply(Matrix, Vector) \
        StaticVectorMatrixReferenceInverseTransposedMultiply(Matrix, Vector)

#define VectorMatrixMultiply(Matrix, Vector) \
        StaticVectorMatrixMultiply(Matrix, Vector)

#define VectorMatrixTransposedMultiply(Matrix, Vector) \
        StaticVectorMatrixTransposedMultiply(Matrix, Vector)

#define VectorMatrixInverseMultiply(Matrix, Vector) \
        StaticVectorMatrixInverseMultiply(Matrix, Vector)

#define VectorMatrixInverseTransposedMultiply(Matrix, Vector) \
        StaticVectorMatrixInverseTransposedMultiply(Matrix, Vector)
#endif

SFORCEINLINE
VECTOR3
VectorMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    if (Matrix == NULL)
    {
        return Vector;
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

    Product = VectorCreate(X, Y, Z);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceMultiply(&Matrix->MatrixReference, Vector);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixReferenceTransposedMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    if (Matrix == NULL)
    {
        return Vector;
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

    Product = VectorCreate(X, Y, Z);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceTransposedMultiply(&Matrix->MatrixReference, Vector);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceMultiply(Matrix->Inverse, Vector);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceInverseMultiply(&Matrix->MatrixReference, Vector);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixReferenceInverseTransposedMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceTransposedMultiply(Matrix->Inverse, Vector);

    return Product;
}

SFORCEINLINE
VECTOR3
VectorMatrixInverseTransposedMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Product;

    if (Matrix == NULL)
    {
        return Vector;
    }

    Product = VectorMatrixReferenceInverseTransposedMultiply(&Matrix->MatrixReference, Vector);

    return Product;
}

#ifdef _IRIS_EXPORT_VECTOR_ROUTINES_
#undef VectorMatrixReferenceMultiply
#undef VectorMatrixReferenceTransposedMultiply
#undef VectorMatrixReferenceInverseMultiply
#undef VectorMatrixReferenceInverseTransposedMultiply
#undef VectorMatrixMultiply
#undef VectorMatrixTransposedMultiply
#undef VectorMatrixInverseMultiply
#undef VectorMatrixInverseTransposedMultiply
#endif

#endif // _VECTOR_IRIS_INTERNAL_