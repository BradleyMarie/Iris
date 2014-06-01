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
PointMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCPOINT3 Point,
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

#ifdef _IRIS_EXPORT_POINT_ROUTINES_
#undef PointMatrixMultiply
#endif

#endif // _POINT_IRIS_INTERNAL_