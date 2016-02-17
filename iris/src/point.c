/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    point.c

Abstract:

    This file contains the definitions for the point type.

--*/

#define _IRIS_EXPORT_POINT_ROUTINES_
#include <irisp.h>

POINT3
PointMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ POINT3 Point
    )
{
    return StaticPointMatrixReferenceMultiply(Matrix, Point);
}

POINT3
PointMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ POINT3 Point
    )
{
    return StaticPointMatrixReferenceInverseMultiply(Matrix, Point);
}

POINT3
PointMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    )
{
    return StaticPointMatrixMultiply(Matrix, Point);
}

POINT3
PointMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ POINT3 Point
    )
{
    return StaticPointMatrixInverseMultiply(Matrix, Point);
}