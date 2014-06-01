/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    ray.c

Abstract:

    This module implements the Iris ray routines.

--*/

#define _IRIS_EXPORT_RAY_ROUTINES_
#include <irisp.h>

IRISAPI
VOID
RayMatrixMultiply(
    _In_ PCMATRIX Multiplicand0,
    _In_ PCRAY Multiplicand1,
    _Out_ PRAY Product
    )
{
    StaticRayMatrixMultiply(Multiplicand0, Multiplicand1, Product);
}