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

#ifdef _IRIS_EXPORT_RAY_ROUTINES_
#define RayMatrixMultiply(Matrix, Vector, Product) \
        StaticRayMatrixMultiply(Matrix, Vector, Product)
#endif

SFORCEINLINE
VOID
RayMatrixMultiply(
	_In_ PCMATRIX Multiplicand0,
    _In_ PCRAY Multiplicand1,
    _Out_ PRAY Product
    )
{
    ASSERT(Multiplicand0 != NULL);
    ASSERT(Multiplicand1 != NULL);
    ASSERT(Product != NULL);

	PointMatrixMultiply(Multiplicand0,
                        &Multiplicand1->Origin, 
                        &Product->Origin);

    VectorMatrixMultiply(Multiplicand0,
                         &Multiplicand1->Direction, 
                         &Product->Direction);
}

#ifdef _IRIS_EXPORT_RAY_ROUTINES_
#undef RayMatrixMultiply
#endif

#endif // _RAY_IRIS_INTERNAL_