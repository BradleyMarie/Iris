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
    __out PPOINT3 Point,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
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
    __out PPOINT3 Point,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z,
    __in FLOAT W
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
    __in PPOINT3 Minuend,
    __in PPOINT3 Subtrahend,
    __out PVECTOR3 Difference
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
    __in PPOINT3 Minuend,
    __in PVECTOR3 Subtrahend,
    __out PPOINT3 Difference
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
    __in PPOINT3 Addend0,
    __in PVECTOR3 Addend1,
    __out PPOINT3 Sum
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
    __in PMATRIX Matrix,
    __in PPOINT3 Point,
    __out PPOINT3 Product
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