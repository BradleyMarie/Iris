/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    ray.c

Abstract:

    This file contains the definitions for the ray type.

--*/

#define _IRIS_EXPORT_RAY_ROUTINES_
#include <irisp.h>

RAY
RayMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ RAY Point
    )
{
    return StaticRayMatrixReferenceMultiply(Matrix, Point);
}

RAY
RayMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Matrix,
    _In_ RAY Point
    )
{
    return StaticRayMatrixReferenceInverseMultiply(Matrix, Point);
}

RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ RAY Point
    )
{
    return StaticRayMatrixMultiply(Matrix, Point);
}

RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX Matrix,
    _In_ RAY Point
    )
{
    return StaticRayMatrixInverseMultiply(Matrix, Point);
}