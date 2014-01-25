/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_vector.h

Abstract:

    This file contains the definitions for the vector type.

--*/

#include <iris.h>

#ifndef _VECTOR_IRIS_
#define _VECTOR_IRIS_

//
// Types
//

typedef struct _VECTOR3 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} VECTOR3, *PVECTOR3;

typedef CONST VECTOR3* PCVECTOR3;

//
// Defines
//

#define VECTOR_X_AXIS 0
#define VECTOR_Y_AXIS 1
#define VECTOR_Z_AXIS 2

typedef UINT8 VECTOR_AXIS;
typedef VECTOR_AXIS *PVECTOR_AXIS;

//
// Functions
//

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
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));

    Vector->X = X;
    Vector->Y = Y;
    Vector->Z = Z;
}

SFORCEINLINE
VOID
VectorAdd(
    _In_ PCVECTOR3 Addend0,
    _In_ PCVECTOR3 Addend1,
    _Out_ PVECTOR3 Sum
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Addend0 != NULL);
    ASSERT(Addend1 != NULL);
    ASSERT(Sum != NULL);

    X = Addend0->X + Addend1->X;
    Y = Addend0->Y + Addend1->Y;
    Z = Addend0->Z + Addend1->Z;

    VectorInitialize(Sum, X, Y, Z);
}

SFORCEINLINE
VOID
VectorSubtract(
    _In_ PCVECTOR3 Minuend,
    _In_ PCVECTOR3 Subtrahend,
    _Out_ PVECTOR3 Difference
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Minuend != NULL);
    ASSERT(Subtrahend != NULL);
    ASSERT(Difference != NULL);

    X = Minuend->X - Subtrahend->X;
    Y = Minuend->Y - Subtrahend->Y;
    Z = Minuend->Z - Subtrahend->Z;

    VectorInitialize(Difference, X, Y, Z);
}

SFORCEINLINE
VOID
VectorScale(
    _In_ PCVECTOR3 Vector,
    _In_ FLOAT Scalar,
    _Out_ PVECTOR3 ScaledVector
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Vector != NULL);
    ASSERT(IsNormalFloat(Scalar));
    ASSERT(IsFiniteFloat(Scalar));
    ASSERT(ScaledVector != NULL);

    X = Vector->X * Scalar;
    Y = Vector->Y * Scalar;
    Z = Vector->Z * Scalar;

    VectorInitialize(ScaledVector, X, Y, Z);
}

SFORCEINLINE
VOID
VectorShrink(
    _In_ PCVECTOR3 Vector,
    _In_ FLOAT Factor,
    _Out_ PVECTOR3 ShrunkVector
    )
{
    FLOAT Scalar;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Vector != NULL);
    ASSERT(IsNormalFloat(Factor));
    ASSERT(IsFiniteFloat(Factor));
    ASSERT(Factor != (FLOAT)0.0);
    ASSERT(ShrunkVector != NULL);

    Scalar = (FLOAT)1.0 / Factor;
    X = Vector->X * Scalar;
    Y = Vector->Y * Scalar;
    Z = Vector->Z * Scalar;

    VectorInitialize(ShrunkVector, X, Y, Z);
}

SFORCEINLINE
FLOAT
VectorDotProduct(
    _In_ PCVECTOR3 Operand0,
    _In_ PCVECTOR3 Operand1
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Operand0 != NULL);
    ASSERT(Operand1 != NULL);

    X = Operand0->X * Operand1->X;
    Y = Operand0->Y * Operand1->Y;
    Z = Operand0->Z * Operand1->Z;

    return X + Y + Z;
}

SFORCEINLINE
VOID
VectorCrossProduct(
    _In_ PCVECTOR3 Operand0,
    _In_ PCVECTOR3 Operand1,
    _Out_ PVECTOR3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Operand0 != NULL);
    ASSERT(Operand1 != NULL);
    ASSERT(Product != NULL);

    X = Operand0->Y * Operand1->Z - Operand0->Z * Operand1->Y;
    Y = Operand0->Z * Operand1->X - Operand0->X * Operand1->Z;
    Z = Operand0->X * Operand1->Y - Operand0->Y * Operand1->X;

    VectorInitialize(Product, X, Y, Z);
}

SFORCEINLINE
FLOAT
VectorLength(
    _In_ PCVECTOR3 Vector
    )
{
    FLOAT DotProduct;

    ASSERT(Vector != NULL);

    DotProduct = VectorDotProduct(Vector, Vector);

    return SqrtFloat(DotProduct);
}

SFORCEINLINE
VOID
VectorNormalize(
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 NormalizedVector
    )
{
    FLOAT Length;

    ASSERT(Vector != NULL);

    Length = VectorDotProduct(Vector, Vector);

    ASSERT(Length != (FLOAT)0.0);

    VectorShrink(Vector, Length, NormalizedVector);
}

SFORCEINLINE
VECTOR_AXIS
VectorDominantAxis(
    _In_ PCVECTOR3 Vector
    )
{
    if (AbsFloat(Vector->X) > AbsFloat(Vector->Y) &&
        AbsFloat(Vector->X) > AbsFloat(Vector->Z))
    {
        return VECTOR_X_AXIS;
    }
    else if (AbsFloat(Vector->Y) > AbsFloat(Vector->Z))
    {
        return VECTOR_Y_AXIS;
    }
    else
    {
        return VECTOR_Z_AXIS;
    }
}

SFORCEINLINE
VECTOR_AXIS
VectorDiminishedAxis(
    _In_ PCVECTOR3 Vector
    )
{
    if (AbsFloat(Vector->X) < AbsFloat(Vector->Y) &&
        AbsFloat(Vector->X) < AbsFloat(Vector->Z))
    {
        return VECTOR_X_AXIS;
    }
    else if (AbsFloat(Vector->Y) < AbsFloat(Vector->Z))
    {
        return VECTOR_Y_AXIS;
    }
    else
    {
        return VECTOR_Z_AXIS;
    }
}

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

#endif // _VECTOR_IRIS_