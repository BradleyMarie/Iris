/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_ray.h

Abstract:

    This module implements the Iris ray routines.

--*/

#include <iris.h>

#ifndef _RAY_IRIS_
#define _RAY_IRIS_

//
// Types
//

typedef struct _RAY {
    POINT3 Origin;
    VECTOR3 Direction;
    FLOAT Time;
} RAY, *PRAY;

//
// Functions
//

SFORCEINLINE
VOID
RayInitialize(
    __out PRAY Ray,
    __in PPOINT3 Origin,
    __in PVECTOR3 Direction
    )
{
    ASSERT(Ray != NULL);
    ASSERT(Origin != NULL);
    ASSERT(Direction != NULL);

    Ray->Origin = *Origin;
    Ray->Direction = *Direction;
}

SFORCEINLINE
VOID
RayEndpoint(
    __in PRAY Ray,
    __in FLOAT Distance,
    __out PPOINT3 Endpoint
    )
{
    VECTOR3 Vector;

    ASSERT(Ray != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(Endpoint != NULL);

    VectorScale(&Ray->Direction, Distance, &Vector);

    PointVectorAdd(&Ray->Origin, &Vector, Endpoint);
}

SFORCEINLINE
VOID
RayMatrixMultiply(
	__in PMATRIX Multiplicand0,
    __in PRAY Multiplicand1,
    __out PRAY Product
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

#endif // _RAY_IRIS_