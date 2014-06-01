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
VOID
PointInitialize(
    _Out_ PPOINT3 Point,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    ASSERT(Point != NULL);
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));

    Point->X = X;
    Point->Y = Y;
    Point->Z = Z;
}

SFORCEINLINE
VOID
PointInitializeScaled(
    _Out_ PPOINT3 Point,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z,
    _In_ FLOAT W
    )
{
    FLOAT Scalar;

    ASSERT(Point != NULL);
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));
    ASSERT(IsZeroFloat(W) == FALSE);
    ASSERT(IsNormalFloat(W));
    ASSERT(IsFiniteFloat(W));

    Scalar = (FLOAT) 1.0 / W;

    PointInitialize(Point, X * Scalar, Y * Scalar, Z * Scalar);
}

SFORCEINLINE
VOID
PointSubtract(
    _In_ PCPOINT3 Minuend,
    _In_ PCPOINT3 Subtrahend,
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
PointVectorSubtract(
    _In_ PCPOINT3 Minuend,
    _In_ PCVECTOR3 Subtrahend,
    _Out_ PPOINT3 Difference
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

    PointInitialize(Difference, X, Y, Z);
}

SFORCEINLINE
VOID
PointVectorAdd(
    _In_ PCPOINT3 Addend0,
    _In_ PCVECTOR3 Addend1,
    _Out_ PPOINT3 Sum
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

    PointInitialize(Sum, X, Y, Z);
}

SFORCEINLINE
VOID
PointVectorAddScaled(
    _In_ PCPOINT3 Point,
    _In_ PCVECTOR3 Vector,
    _In_ FLOAT Scalar,
    _Out_ PPOINT3 Sum
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Point != NULL);
    ASSERT(Vector != NULL);
    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);
    ASSERT(Sum != NULL);

    X = FmaFloat(Scalar, Vector->X, Point->X);
    Y = FmaFloat(Scalar, Vector->Y, Point->Y);
    Z = FmaFloat(Scalar, Vector->Z, Point->Z);

    PointInitialize(Sum, X, Y, Z);
}

SFORCEINLINE
VOID
PointVectorSubtractScaled(
    _In_ PCPOINT3 Point,
    _In_ PCVECTOR3 Vector,
    _In_ FLOAT Scalar,
    _Out_ PPOINT3 Sum
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Point != NULL);
    ASSERT(Vector != NULL);
    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);
    ASSERT(Sum != NULL);

    Scalar = -Scalar;

    X = FmaFloat(Scalar, Vector->X, Point->X);
    Y = FmaFloat(Scalar, Vector->Y, Point->Y);
    Z = FmaFloat(Scalar, Vector->Z, Point->Z);

    PointInitialize(Sum, X, Y, Z);
}

#ifndef _DISABLE_IRIS_POINT_EXPORTS_

IRISAPI
VOID
PointMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCPOINT3 Point,
    _Out_ PPOINT3 Product
    );

#endif // _DISABLE_IRIS_POINT_EXPORTS

#endif // _POINT_IRIS_