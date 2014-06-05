/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    point.c

Abstract:

    This file contains the definitions for the point type.

--*/

#define _IRIS_EXPORT_POINT_ROUTINES_
#include <irisp.h>

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PointMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ POINT3 Point,
    _Out_ PPOINT3 Product
    )
{
    return StaticPointMatrixMultiply(Matrix, Point, Product);
}