/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_point.h

Abstract:

    This file contains the definitions for the point type.

--*/

#include <iris.h>

#ifndef _VECTOR_IRIS_
#define _VECTOR_IRIS_

//
// Types
//

typedef struct _POINT {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} POINT, *PPOINT;

//
// Functions
//

SFORCEINLINE
VOID
PointInitialize(
    __out PPOINT Point,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    )
{
    ASSERT(Point != NULL);
    ASSERT(!IsNaNFloat(X));
    ASSERT(!IsInfFloat(X));
    ASSERT(!IsNaNFloat(Y));
    ASSERT(!IsInfFloat(Y));
    ASSERT(!IsNaNFloat(Z));
    ASSERT(!IsInfFloat(Z));

    Point->X = X;
    Point->Y = Y;
    Point->Z = Z;
}

SFORCEINLINE
VOID
PointInitializeScaled(
    __out PPOINT Point,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z,
    __in FLOAT W
    )
{
    FLOAT Scalar;

    ASSERT(Point != NULL);
    ASSERT(!IsNaNFloat(X));
    ASSERT(!IsInfFloat(X));
    ASSERT(!IsNaNFloat(Y));
    ASSERT(!IsInfFloat(Y));
    ASSERT(!IsNaNFloat(Z));
    ASSERT(!IsInfFloat(Z));
    ASSERT(W != (FLOAT)0.0);
    ASSERT(!IsNaNFloat(W));
    ASSERT(!IsInfFloat(W));

    Scalar = (FLOAT)1.0 / W;

    Point->X = X * W;
    Point->Y = Y * W;
    Point->Z = Z * W;
}

SFORCEINLINE
VOID
PointSubtract(
    __in PPOINT Minuend,
    __in PPOINT Subtrahend,
    __out PVECTOR Difference
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
    __in PPOINT Minuend,
    __in PVECTOR Subtrahend,
    __out PPOINT Difference
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
    __in PPOINT Addend0,
    __in PVECTOR Addend1,
    __out PPOINT Sum
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
    __in PPOINT Point,
    __out PPOINT Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Matrix != NULL);
    ASSERT(Point != NULL);
    ASSERT(Product != NULL);

    X = Matrix->M[0][0] * Point->X + 
        Matrix->M[0][1] * Point->Y + 
        Matrix->M[0][2] * Point->Z;

    Y = Matrix->M[1][0] * Point->X + 
        Matrix->M[1][1] * Point->Y + 
        Matrix->M[1][2] * Point->Z;

    Z = Matrix->M[2][0] * Point->X + 
        Matrix->M[2][1] * Point->Y + 
        Matrix->M[2][2] * Point->Z;

    PointInitialize(Product, X, Y, Z);
}

#endif // _VECTOR_IRIS_