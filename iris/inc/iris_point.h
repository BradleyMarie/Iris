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
    ASSERT(W != (FLOAT)0.0);
    ASSERT(IsNormalFloat(W));
    ASSERT(IsFiniteFloat(W));

    Scalar = (FLOAT)1.0 / W;

	PointInitialize(Point, X * W, Y * W, Z * W);
}

SFORCEINLINE
VOID
PointSubtract(
    _In_ PPOINT3 Minuend,
    _In_ PPOINT3 Subtrahend,
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
    _In_ PPOINT3 Minuend,
    _In_ PVECTOR3 Subtrahend,
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
    _In_ PPOINT3 Addend0,
    _In_ PVECTOR3 Addend1,
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
PointMatrixMultiply(
    _In_ PMATRIX Matrix,
    _In_ PPOINT3 Point,
    _Out_ PPOINT3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
	FLOAT W;

    ASSERT(Matrix != NULL);
    ASSERT(Point != NULL);
    ASSERT(Product != NULL);

    X = Matrix->M[0][0] * Point->X + 
        Matrix->M[0][1] * Point->Y + 
        Matrix->M[0][2] * Point->Z +
        Matrix->M[0][3];

    Y = Matrix->M[1][0] * Point->X + 
        Matrix->M[1][1] * Point->Y + 
        Matrix->M[1][2] * Point->Z +
        Matrix->M[1][3];

    Z = Matrix->M[2][0] * Point->X + 
        Matrix->M[2][1] * Point->Y + 
        Matrix->M[2][2] * Point->Z +
        Matrix->M[2][3];

    W = Matrix->M[3][0] * Point->X + 
        Matrix->M[3][1] * Point->Y + 
        Matrix->M[3][2] * Point->Z + 
        Matrix->M[3][3];

    PointInitializeScaled(Product, X, Y, Z, W);
}

#endif // _POINT_IRIS_