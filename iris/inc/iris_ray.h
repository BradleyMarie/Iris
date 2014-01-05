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

typedef CONST RAY *PCRAY;

//
// Functions
//

SFORCEINLINE
VOID
RayInitialize(
    _Out_ PRAY Ray,
    _In_ PCPOINT3 Origin,
    _In_ PCVECTOR3 Direction
    )
{
    ASSERT(Ray != NULL);
    ASSERT(Origin != NULL);
    ASSERT(Direction != NULL);

    Ray->Origin = *Origin;
    Ray->Direction = *Direction;
	Ray->Time = (FLOAT)0.0;
}

SFORCEINLINE
VOID
RayEndpoint(
    _In_ PCRAY Ray,
    _In_ FLOAT Distance,
    _Out_ PPOINT3 Endpoint
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

#endif // _RAY_IRIS_