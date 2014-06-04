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

typedef CONST VECTOR3 *PCVECTOR3;

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

SFORCEINLINE
VOID
VectorNegate(
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Negation
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Vector != NULL);
    ASSERT(Negation != NULL);

    X = -Vector->X;
    Y = -Vector->Y;
    Z = -Vector->Z;

    VectorInitialize(Negation, X, Y, Z);
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
VectorAddScaled(
    _In_ PCVECTOR3 Addend0,
    _In_ PCVECTOR3 Addend1,
    _In_ FLOAT Scalar,
    _Out_ PVECTOR3 Sum
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Addend0 != NULL);
    ASSERT(Addend1 != NULL);
    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);
    ASSERT(Sum != NULL);

    X = FmaFloat(Scalar, Addend1->X, Addend0->X);
    Y = FmaFloat(Scalar, Addend1->Y, Addend0->Y);
    Z = FmaFloat(Scalar, Addend1->Z, Addend0->Z);

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
    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);
    ASSERT(ScaledVector != NULL);

    X = Vector->X * Scalar;
    Y = Vector->Y * Scalar;
    Z = Vector->Z * Scalar;

    VectorInitialize(ScaledVector, X, Y, Z);
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
VectorNormalizeWithLength(
    _In_ PCVECTOR3 Vector,
    _Out_ PFLOAT OldLength,
    _Out_ PVECTOR3 NormalizedVector
    )
{
    FLOAT Scalar;

    ASSERT(Vector != NULL);
    ASSERT(OldLength != NULL);
    ASSERT(NormalizedVector != NULL);

    *OldLength = VectorLength(Vector);

    ASSERT(IsZeroFloat(*OldLength) == FALSE);

    Scalar = (FLOAT) 1.0 / *OldLength;

    VectorScale(Vector, Scalar, NormalizedVector);
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
    ASSERT(NormalizedVector != NULL);

    VectorNormalizeWithLength(Vector,
                              &Length,
                              NormalizedVector);
}

SFORCEINLINE
BOOL
VectorValidate(
    _In_opt_ PCVECTOR3 Vector
    )
{
    if (Vector == NULL ||
        IsNormalFloat(Vector->X) == FALSE ||
        IsFiniteFloat(Vector->X) == FALSE ||
        IsNormalFloat(Vector->Y) == FALSE ||
        IsFiniteFloat(Vector->Y) == FALSE ||
        IsNormalFloat(Vector->Z) == FALSE ||
        IsFiniteFloat(Vector->Z) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

SFORCEINLINE
VECTOR_AXIS
VectorDominantAxis(
    _In_ PCVECTOR3 Vector
    )
{
    FLOAT AbsoluteValueX;
    FLOAT AbsoluteValueY;
    FLOAT AbsoluteValueZ;

    AbsoluteValueX = AbsFloat(Vector->X);
    AbsoluteValueY = AbsFloat(Vector->Y);
    AbsoluteValueZ = AbsFloat(Vector->Z);

    if (AbsoluteValueX > AbsoluteValueY &&
        AbsoluteValueX > AbsoluteValueZ)
    {
        return VECTOR_X_AXIS;
    }
    else if (AbsoluteValueY > AbsoluteValueZ)
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
    FLOAT AbsoluteValueX;
    FLOAT AbsoluteValueY;
    FLOAT AbsoluteValueZ;

    AbsoluteValueX = AbsFloat(Vector->X);
    AbsoluteValueY = AbsFloat(Vector->Y);
    AbsoluteValueZ = AbsFloat(Vector->Z);

    if (AbsoluteValueX < AbsoluteValueY &&
        AbsoluteValueX < AbsoluteValueZ)
    {
        return VECTOR_X_AXIS;
    }
    else if (AbsoluteValueY < AbsoluteValueZ)
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
VectorReflect(
    _In_ PCVECTOR3 Incident,
    _In_ PCVECTOR3 Normal,
    _Out_ PVECTOR3 NormalizedReflected
    )
{
    VECTOR3 Reflected;
    FLOAT Scalar;

    ASSERT(Incident != NULL);
    ASSERT(Normal != NULL);
    ASSERT(NormalizedReflected != NULL);

    Scalar = (FLOAT) 2.0 * VectorDotProduct(Normal, Incident);

    VectorScale(Normal, Scalar, &Reflected);
    VectorSubtract(Incident, &Reflected, &Reflected);

    VectorNormalize(&Reflected, NormalizedReflected);
}

SFORCEINLINE
VOID
VectorHalfAngle(
    PCVECTOR3 Vector0,
    PCVECTOR3 Vector1,
    PVECTOR3 Result
    )
{
    ASSERT(Vector0 != NULL);
    ASSERT(Vector1 != NULL);
    ASSERT(Result != NULL);

    VectorAdd(Vector0, Vector1, Result);
    VectorNormalize(Result, Result);
}

#ifndef _DISABLE_IRIS_VECTOR_EXPORTS_

IRISAPI
VOID
VectorMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    );

IRISAPI
VOID
VectorMatrixTransposedMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCVECTOR3 Vector,
    _Out_ PVECTOR3 Product
    );

#endif // _DISABLE_IRIS_VECTOR_EXPORTS_

#endif // _VECTOR_IRIS_