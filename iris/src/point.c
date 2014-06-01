/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    point.c

Abstract:

    This file contains the definitions for the point type.

--*/

#define _IRIS_EXPORT_POINT_ROUTINES_
#include <irisp.h>

VOID
PointMatrixMultiply(
    _In_ PCMATRIX Matrix,
    _In_ PCPOINT3 Point,
    _Out_ PPOINT3 Product
    )
{
    StaticPointMatrixMultiply(Matrix, Point, Product);
}