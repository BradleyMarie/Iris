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
#define RayMatrixReferenceMultiply(Matrix, Vector) \
        StaticRayMatrixReferenceMultiply(Matrix, Vector)

#define RayMatrixReferenceInverseMultiply(Matrix, Vector) \
        StaticRayMatrixReferenceInverseMultiply(Matrix, Vector)

#define RayMatrixMultiply(Matrix, Vector) \
        StaticRayMatrixMultiply(Matrix, Vector)

#define RayMatrixInverseMultiply(Matrix, Vector) \
        StaticRayMatrixInverseMultiply(Matrix, Vector)
#endif

SFORCEINLINE
RAY
RayMatrixReferenceMultiply(
    _In_opt_ PCMATRIX_REFERENCE Multiplicand0,
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

    MultipliedOrigin = PointMatrixReferenceMultiply(Multiplicand0,
                                                    Multiplicand1.Origin);

    MultipliedDirection = VectorMatrixReferenceMultiply(Multiplicand0,
                                                        Multiplicand1.Direction);

    Product = RayCreate(MultipliedOrigin, MultipliedDirection);

    return Product;
}

SFORCEINLINE
RAY
RayMatrixReferenceInverseMultiply(
    _In_opt_ PCMATRIX_REFERENCE Multiplicand0,
    _In_ RAY Multiplicand1
    )
{
    RAY Product;

    if (Multiplicand0 == NULL)
    {
        return Multiplicand1;
    }

    Product = RayMatrixReferenceMultiply(Multiplicand0->Inverse, Multiplicand1);

    return Product;
}

SFORCEINLINE
RAY
RayMatrixMultiply(
    _In_opt_ PCMATRIX Multiplicand0,
    _In_ RAY Multiplicand1
    )
{
    RAY Product;

    if (Multiplicand0 == NULL)
    {
        return Multiplicand1;
    }

    Product = RayMatrixReferenceMultiply(&Multiplicand0->MatrixReference, Multiplicand1);

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

    Product = RayMatrixReferenceMultiply(Multiplicand0->MatrixReference.Inverse, Multiplicand1);

    return Product;
}

#ifdef _IRIS_EXPORT_RAY_ROUTINES_
#undef RayMatrixReferenceMultiply
#undef RayMatrixReferenceInverseMultiply
#undef RayMatrixMultiply
#undef RayMatrixInverseMultiply
#endif

#endif // _RAY_IRIS_INTERNAL_