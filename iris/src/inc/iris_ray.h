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
POINT3
RayEndpoint(
    _In_ RAY Ray,
    _In_ FLOAT Distance
    )
{
    POINT3 Endpoint;

    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));

    Endpoint = PointVectorAddScaled(Ray.Origin,
                                    Ray.Direction,
                                    Distance);

    return Endpoint;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayMatrixMultiply(
    _In_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1,
    _Out_ PRAY Product
    )
{
    if (Multiplicand0 == NULL ||
        Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

	PointMatrixMultiply(Multiplicand0,
                        Multiplicand1.Origin, 
                        &Product->Origin);

    VectorMatrixMultiply(Multiplicand0,
                         Multiplicand1.Direction, 
                         &Product->Direction);

    return ISTATUS_SUCCESS;
}

#endif // _RAY_IRIS_INTERNAL_