/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_point.h

Abstract:

    This file contains the internal definitions for the point type.

--*/

#include <irisp.h>

#ifndef _POINT_IRIS_INTERNAL_
#define _POINT_IRIS_INTERNAL_

#ifdef _IRIS_EXPORT_POINT_ROUTINES_
#define PointMatrixMultiply(Matrix, Vector, Product) \
        StaticPointMatrixMultiply(Matrix, Vector, Product)
#endif

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

    ASSERT(IsZeroFloat(W) == FALSE);

    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));
    ASSERT(IsNormalFloat(W));
    ASSERT(IsFiniteFloat(W));

    Scalar = (FLOAT) 1.0 / W;

    Point->X = X * Scalar;
    Point->Y = Y * Scalar;
    Point->Z = Z * Scalar;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PointMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ POINT3 Point,
    _Out_ PPOINT3 Product
    )
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT W;

    if (Matrix == NULL ||
        Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    X = Matrix->M[0][0] * Point.X + 
        Matrix->M[0][1] * Point.Y + 
        Matrix->M[0][2] * Point.Z +
        Matrix->M[0][3];

    Y = Matrix->M[1][0] * Point.X + 
        Matrix->M[1][1] * Point.Y + 
        Matrix->M[1][2] * Point.Z +
        Matrix->M[1][3];

    Z = Matrix->M[2][0] * Point.X + 
        Matrix->M[2][1] * Point.Y + 
        Matrix->M[2][2] * Point.Z +
        Matrix->M[2][3];

    W = Matrix->M[3][0] * Point.X + 
        Matrix->M[3][1] * Point.Y + 
        Matrix->M[3][2] * Point.Z + 
        Matrix->M[3][3];

    PointInitializeScaled(Product, X, Y, Z, W);

    return ISTATUS_SUCCESS;
}

#ifdef _IRIS_EXPORT_POINT_ROUTINES_
#undef PointMatrixMultiply
#endif

#endif // _POINT_IRIS_INTERNAL_