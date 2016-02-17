/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_point.h

Abstract:

    This file contains the definitions for the point type.

--*/

#include <iris.h>

#ifndef _POINT_IRIS_
#define _POINT_IRIS_

//
// Types
//

typedef struct _POINT3 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} POINT3, *PPOINT3;

typedef CONST POINT3 *PCPOINT3;

//
// Functions
//

SFORCEINLINE
POINT3
PointCreate(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    POINT3 Point;

    ASSERT(IsFiniteFloat(X) != FALSE);
    ASSERT(IsFiniteFloat(Y) != FALSE);
    ASSERT(IsFiniteFloat(Z) != FALSE);

    Point.X = X;
    Point.Y = Y;
    Point.Z = Z;

    return Point;
}

SFORCEINLINE
VECTOR3
PointSubtract(
    _In_ POINT3 Minuend,
    _In_ POINT3 Subtrahend
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
POINT3
PointVectorSubtract(
    _In_ POINT3 Minuend,
    _In_ VECTOR3 Subtrahend
    )
{
    POINT3 Difference;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Minuend.X - Subtrahend.X;
    Y = Minuend.Y - Subtrahend.Y;
    Z = Minuend.Z - Subtrahend.Z;

    Difference = PointCreate(X, Y, Z);

    return Difference;
}

SFORCEINLINE
POINT3
PointVectorAdd(
    _In_ POINT3 Addend0,
    _In_ VECTOR3 Addend1
    )
{
    POINT3 Sum;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    X = Addend0.X + Addend1.X;
    Y = Addend0.Y + Addend1.Y;
    Z = Addend0.Z + Addend1.Z;

    Sum = PointCreate(X, Y, Z);

    return Sum;
}

SFORCEINLINE
POINT3
PointVectorAddScaled(
    _In_ POINT3 Point,
    _In_ VECTOR3 Vector,
    _In_ FLOAT Scalar
    )
{
    POINT3 Sum;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    X = FmaFloat(Scalar, Vector.X, Point.X);
    Y = FmaFloat(Scalar, Vector.Y, Point.Y);
    Z = FmaFloat(Scalar, Vector.Z, Point.Z);

    Sum = PointCreate(X, Y, Z);

    return Sum;
}

SFORCEINLINE
POINT3
PointVectorSubtractScaled(
    _In_ POINT3 Point,
    _In_ VECTOR3 Vector,
    _In_ FLOAT Scalar
    )
{
    POINT3 Difference;
    FLOAT NegatedScalar;

    NegatedScalar = -Scalar;

    Difference = PointVectorAddScaled(Point,
                                      Vector, 
                                      NegatedScalar);

    return Difference;
}

SFORCEINLINE
BOOL
PointValidate(
    _In_ POINT3 Point
    )
{
    if (IsFiniteFloat(Point.X) == FALSE ||
        IsFiniteFloat(Point.Y) == FALSE ||
        IsFiniteFloat(Point.Z) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

#ifndef _DISABLE_IRIS_POINT_EXPORTS_

IRISAPI
POINT3
PointMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ POINT3 Point
    );

IRISAPI
POINT3
PointMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ POINT3 Point
    );

IRISAPI
POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    );

IRISAPI
POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    );

#endif // _DISABLE_IRIS_POINT_EXPORTS

#endif // _POINT_IRIS_