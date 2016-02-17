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
RAY
RayCreate(
    _In_ POINT3 Origin,
    _In_ VECTOR3 Direction
    )
{
    RAY Ray;

    Ray.Origin = Origin;
    Ray.Direction = Direction;
    Ray.Time = (FLOAT) 0.0;

    return Ray;
}

SFORCEINLINE
POINT3
RayEndpoint(
    _In_ RAY Ray,
    _In_ FLOAT Distance
    )
{
    POINT3 Endpoint;

    ASSERT(IsFiniteFloat(Distance) != FALSE);

    Endpoint = PointVectorAddScaled(Ray.Origin,
                                    Ray.Direction,
                                    Distance);

    return Endpoint;
}

SFORCEINLINE
RAY
RayNormalize(
    _In_ RAY Ray
    )
{
    VECTOR3 NormalizedDirection;
    RAY NormalizedRay;

    NormalizedDirection = VectorNormalize(Ray.Direction, NULL, NULL);
    NormalizedRay = RayCreate(Ray.Origin, Ray.Direction);

    return NormalizedRay;
}

SFORCEINLINE
BOOL
RayValidate(
    _In_ RAY Ray
    )
{
    if (PointValidate(Ray.Origin) == FALSE ||
        VectorValidate(Ray.Direction) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

#ifndef _DISABLE_IRIS_RAY_EXPORTS_

IRISAPI
RAY
RayMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Multiplicand0,
    _In_ RAY Multiplicand1
    );

IRISAPI
RAY
RayMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Multiplicand0,
    _In_ RAY Multiplicand1
    );

IRISAPI
RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1
    );

IRISAPI
RAY
RayMatrixInverseMultiply(
    _In_opt_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1
    );

#endif // _DISABLE_IRIS_RAY_EXPORTS_

#endif // _RAY_IRIS_