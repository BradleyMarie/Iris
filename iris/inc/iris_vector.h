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
VECTOR3
VectorCreate(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    VECTOR3 Result;

    ASSERT(IsNormalFloat(X) != FALSE);
    ASSERT(IsFiniteFloat(X) != FALSE);
    ASSERT(IsNormalFloat(Y) != FALSE);
    ASSERT(IsFiniteFloat(Y) != FALSE);
    ASSERT(IsNormalFloat(Z) != FALSE);
    ASSERT(IsFiniteFloat(Z) != FALSE);

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;

    return Result;
}

SFORCEINLINE
VECTOR3
VectorNegate(
    _In_ VECTOR3 Vector
    )
{
    VECTOR3 Negation;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = -Vector.X;
    Y = -Vector.Y;
    Z = -Vector.Z;

    Negation = VectorCreate(X, Y, Z);

    return Negation;
}

SFORCEINLINE
VECTOR3
VectorAdd(
    _In_ VECTOR3 Addend0,
    _In_ VECTOR3 Addend1
    )
{
    VECTOR3 Sum;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Addend0.X + Addend1.X;
    Y = Addend0.Y + Addend1.Y;
    Z = Addend0.Z + Addend1.Z;

    Sum = VectorCreate(X, Y, Z);

    return Sum;
}

SFORCEINLINE
VECTOR3
VectorAddScaled(
    _In_ VECTOR3 Addend0,
    _In_ VECTOR3 Addend1,
    _In_ FLOAT Scalar
    )
{
    VECTOR3 Sum;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    X = FmaFloat(Scalar, Addend1.X, Addend0.X);
    Y = FmaFloat(Scalar, Addend1.Y, Addend0.Y);
    Z = FmaFloat(Scalar, Addend1.Z, Addend0.Z);

    Sum = VectorCreate(X, Y, Z);

    return Sum;
}

SFORCEINLINE
VECTOR3
VectorSubtract(
    _In_ VECTOR3 Minuend,
    _In_ VECTOR3 Subtrahend
    )
{
    VECTOR3 Difference;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Minuend.X - Subtrahend.X;
    Y = Minuend.Y - Subtrahend.Y;
    Z = Minuend.Z - Subtrahend.Z;

    Difference = VectorCreate(X, Y, Z);

    return Difference;
}

SFORCEINLINE
VECTOR3
VectorScale(
    _In_ VECTOR3 Vector,
    _In_ FLOAT Scalar
    )
{
    VECTOR3 ScaledVector;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);
    ASSERT(IsZeroFloat(Scalar) == FALSE);

    X = Vector.X * Scalar;
    Y = Vector.Y * Scalar;
    Z = Vector.Z * Scalar;

    ScaledVector = VectorCreate(X, Y, Z);

    return ScaledVector;
}

SFORCEINLINE
FLOAT
VectorDotProduct(
    _In_ VECTOR3 Operand0,
    _In_ VECTOR3 Operand1
    )
{
    FLOAT DotProduct;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Operand0.X * Operand1.X;
    Y = Operand0.Y * Operand1.Y;
    Z = Operand0.Z * Operand1.Z;

    DotProduct = X + Y + Z;

    return DotProduct;
}

SFORCEINLINE
VECTOR3
VectorCrossProduct(
    _In_ VECTOR3 Operand0,
    _In_ VECTOR3 Operand1
    )
{
    VECTOR3 CrossProduct;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Operand0.Y * Operand1.Z - Operand0.Z * Operand1.Y;
    Y = Operand0.Z * Operand1.X - Operand0.X * Operand1.Z;
    Z = Operand0.X * Operand1.Y - Operand0.Y * Operand1.X;

    CrossProduct = VectorCreate(X, Y, Z);

    return CrossProduct;
}

SFORCEINLINE
FLOAT
VectorLength(
    _In_ VECTOR3 Vector
    )
{
    FLOAT DotProduct;
    FLOAT Length;

    DotProduct = VectorDotProduct(Vector, Vector);

    Length = SqrtFloat(DotProduct);

    return Length;
}

SFORCEINLINE
VECTOR3
VectorNormalize(
    _In_ VECTOR3 Vector,
    _Out_opt_ PFLOAT OldLength
    )
{
    VECTOR3 Normalized;
    FLOAT Length;
    FLOAT Scalar;

    Length = VectorLength(Vector);

    if (OldLength != NULL)
    {
        *OldLength = Length;
    }

    Scalar = (FLOAT) 1.0 / Length;

    Normalized = VectorScale(Vector, Scalar);

    return Normalized;
}

SFORCEINLINE
BOOL
VectorValidate(
    _In_ VECTOR3 Vector
    )
{
    if (IsZeroFloat(Vector.X) != FALSE &&
        IsZeroFloat(Vector.Y) != FALSE &&
        IsZeroFloat(Vector.Z) != FALSE)
    {
        return FALSE;
    }

    if (IsNormalFloat(Vector.X) == FALSE ||
        IsFiniteFloat(Vector.X) == FALSE ||
        IsNormalFloat(Vector.Y) == FALSE ||
        IsFiniteFloat(Vector.Y) == FALSE ||
        IsNormalFloat(Vector.Z) == FALSE ||
        IsFiniteFloat(Vector.Z) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

SFORCEINLINE
VECTOR_AXIS
VectorDominantAxis(
    _In_ VECTOR3 Vector
    )
{
    FLOAT AbsoluteValueX;
    FLOAT AbsoluteValueY;
    FLOAT AbsoluteValueZ;

    AbsoluteValueX = AbsFloat(Vector.X);
    AbsoluteValueY = AbsFloat(Vector.Y);
    AbsoluteValueZ = AbsFloat(Vector.Z);

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
    _In_ VECTOR3 Vector
    )
{
    FLOAT AbsoluteValueX;
    FLOAT AbsoluteValueY;
    FLOAT AbsoluteValueZ;

    AbsoluteValueX = AbsFloat(Vector.X);
    AbsoluteValueY = AbsFloat(Vector.Y);
    AbsoluteValueZ = AbsFloat(Vector.Z);

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
VECTOR3
VectorReflect(
    _In_ VECTOR3 Incident,
    _In_ VECTOR3 Normal
    )
{
    VECTOR3 Reflected;
    FLOAT Scalar;

    Scalar = (FLOAT) 2.0 * VectorDotProduct(Normal, Incident);

    Reflected = VectorScale(Normal, Scalar);
    Reflected = VectorSubtract(Incident, Reflected);
    Reflected = VectorNormalize(Reflected, NULL);

    return Reflected;
}

SFORCEINLINE
VECTOR3
VectorHalfAngle(
    VECTOR3 Vector0,
    VECTOR3 Vector1
    )
{
    VECTOR3 Result;

    Result = VectorAdd(Vector0, Vector1);
    Result = VectorNormalize(Result, NULL);

    return Result;
}

#ifndef _DISABLE_IRIS_VECTOR_EXPORTS_

IRISAPI
VECTOR3
VectorMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    );

IRISAPI
VECTOR3
VectorMatrixTransposedMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    );

IRISAPI
VECTOR3
VectorMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    );

IRISAPI
VECTOR3
VectorMatrixTransposedInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ VECTOR3 Vector
    );

#endif // _DISABLE_IRIS_VECTOR_EXPORTS_

#endif // _VECTOR_IRIS_