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
#define PointMatrixMultiply(Matrix, Vector) \
        StaticPointMatrixMultiply(Matrix, Vector)

#define PointMatrixInverseMultiply(Matrix, Vector) \
        StaticPointMatrixInverseMultiply(Matrix, Vector)
#endif

SFORCEINLINE
POINT3
PointCreateScaled(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z,
    _In_ FLOAT W
    )
{
    POINT3 Point;
    FLOAT Scalar;

    ASSERT(IsNotZeroFloat(W) != FALSE);

    ASSERT(IsFiniteFloat(X) != FALSE);
    ASSERT(IsFiniteFloat(Y) != FALSE);
    ASSERT(IsFiniteFloat(Z) != FALSE);
    ASSERT(IsFiniteFloat(W) != FALSE);

    Scalar = (FLOAT) 1.0 / W;

    Point.X = X * Scalar;
    Point.Y = Y * Scalar;
    Point.Z = Z * Scalar;

    return Point;
}

SFORCEINLINE
POINT3
PointMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ POINT3 Point
    )
{
    POINT3 Product;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT W;

    if (Matrix == NULL)
    {
        return Point;
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

    Product = PointCreateScaled(X, Y, Z, W);

    return Product;
}

SFORCEINLINE
POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    )
{
    POINT3 Product;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT W;

    if (Matrix == NULL)
    {
        return Point;
    }

    X = Matrix->MatrixReference.M[0][0] * Point.X + 
        Matrix->MatrixReference.M[0][1] * Point.Y + 
        Matrix->MatrixReference.M[0][2] * Point.Z +
        Matrix->MatrixReference.M[0][3];

    Y = Matrix->MatrixReference.M[1][0] * Point.X + 
        Matrix->MatrixReference.M[1][1] * Point.Y + 
        Matrix->MatrixReference.M[1][2] * Point.Z +
        Matrix->MatrixReference.M[1][3];

    Z = Matrix->MatrixReference.M[2][0] * Point.X + 
        Matrix->MatrixReference.M[2][1] * Point.Y + 
        Matrix->MatrixReference.M[2][2] * Point.Z +
        Matrix->MatrixReference.M[2][3];

    W = Matrix->MatrixReference.M[3][0] * Point.X + 
        Matrix->MatrixReference.M[3][1] * Point.Y + 
        Matrix->MatrixReference.M[3][2] * Point.Z + 
        Matrix->MatrixReference.M[3][3];

    Product = PointCreateScaled(X, Y, Z, W);

    return Product;
}

SFORCEINLINE
POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    )
{
    POINT3 Product;

    if (Matrix == NULL)
    {
        return Point;
    }

    Product = PointMatrixReferenceMultiply(Matrix->MatrixReference.Inverse, Point);

    return Product;
}

#ifdef _IRIS_EXPORT_POINT_ROUTINES_
#undef PointMatrixMultiply
#undef PointMatrixInverseMultiply
#endif

#endif // _POINT_IRIS_INTERNAL_