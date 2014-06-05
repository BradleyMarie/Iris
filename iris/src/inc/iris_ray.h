/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_ray.h

Abstract:

    This module implements the internal Iris ray routines.

--*/

#include <irisp.h>

#ifndef _RAY_IRIS_INTERNAL_
#define _RAY_IRIS_INTERNAL_

SFORCEINLINE
RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1
    )
{
    VECTOR3 MultipliedDirection;
    POINT3 MultipliedOrigin;
    RAY Product;

    if (Multiplicand0 == NULL)
    {
        return Multiplicand1;
    }

    MultipliedOrigin = PointMatrixMultiply(Multiplicand0,
                                           Multiplicand1.Origin);

    MultipliedDirection = VectorMatrixMultiply(Multiplicand0,
                                               Multiplicand1.Direction);

    Product = RayCreate(MultipliedOrigin, MultipliedDirection);

    return Product;
}

SFORCEINLINE
RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1
    )
{
    RAY Product;

    if (Multiplicand0 == NULL)
    {
        return Multiplicand1;
    }

    Product = RayMatrixMultiply(Multiplicand0->Inverse, Multiplicand1);

    return Product;
}

#endif // _RAY_IRIS_INTERNAL_