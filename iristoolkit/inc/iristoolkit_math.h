/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_math.h

Abstract:

    This file contains the prototypes for the camera routines.

--*/

#ifndef _MATH_IRIS_TOOLKIT_
#define _MATH_IRIS_TOOLKIT_

#include <iristoolkit.h>
#include <math.h>

//
// Constants
//

#define IRIS_TWO_PI (IRIS_PI * (FLOAT) 2.0)
#define IRIS_INV_PI ((FLOAT) 1.0 / IRIS_PI)
#define IRIS_INV_TWO_PI ((FLOAT) 1.0 / IRIS_TWO_PI)

//
// Macros
//

#define CosineFloat(number) cosf(number)
#define SineFloat(number) sinf(number)

//
// Functions
//

SFORCEINLINE
VECTOR3
IrisToolkitGenerateOrthogonalVector(
    _In_ VECTOR3 NormalizedVector
    )
{
    VECTOR3 OrthogonalVector;
    VECTOR_AXIS ShortestAxis;

    ShortestAxis = VectorDiminishedAxis(NormalizedVector);

    switch (ShortestAxis)
    {
        case VECTOR_X_AXIS:
            OrthogonalVector = VectorCreate((FLOAT) 1.0,
                                            (FLOAT) 0.0,
                                            (FLOAT) 0.0);
            break;
        case VECTOR_Y_AXIS:
            OrthogonalVector = VectorCreate((FLOAT) 1.0,
                                            (FLOAT) 1.0,
                                            (FLOAT) 0.0);
            break;
        default: // VECTOR_Z_AXIS
            OrthogonalVector = VectorCreate((FLOAT) 0.0,
                                            (FLOAT) 0.0,
                                            (FLOAT) 1.0);
            break;
    }

    OrthogonalVector = VectorCrossProduct(NormalizedVector,
                                          OrthogonalVector);

    OrthogonalVector = VectorNormalize(OrthogonalVector, NULL);

    return OrthogonalVector;
}

SFORCEINLINE
VECTOR3
IrisToolkitTransformVector(
    _In_ VECTOR3 NormalizedNormal,
    _In_ VECTOR3 VectorToTransform
    )
{
    VECTOR3 TransformedVector;
    VECTOR3 OrthogonalVector;
    VECTOR3 CrossProduct;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    OrthogonalVector = IrisToolkitGenerateOrthogonalVector(NormalizedNormal);
    CrossProduct = VectorCrossProduct(NormalizedNormal, OrthogonalVector);

    X = OrthogonalVector.X * VectorToTransform.X + 
        CrossProduct.X * VectorToTransform.Y +
        NormalizedNormal.X * VectorToTransform.Z;

    Y = OrthogonalVector.Y * VectorToTransform.X + 
        CrossProduct.Y * VectorToTransform.Y +
        NormalizedNormal.Y * VectorToTransform.Z;

    Z = OrthogonalVector.Z * VectorToTransform.X + 
        CrossProduct.Z * VectorToTransform.Y +
        NormalizedNormal.Z * VectorToTransform.Z;

    TransformedVector = VectorCreate(X, Y, Z);

    return TransformedVector;
}

SFORCEINLINE
ISTATUS
IrisToolkitCosineSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM Rng,
    _Out_ PVECTOR3 RandomVector
    )
{
    FLOAT RandomNumber0;
    FLOAT RandomNumber1;
    FLOAT Radius;
    VECTOR3 Result;
    ISTATUS Status;
    FLOAT Theta;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    ASSERT(Rng != NULL);
    ASSERT(RandomVector != NULL);

    Status = RandomGenerateFloat(Rng,
                                 (FLOAT) 0.0,
                                 (FLOAT) 1.0,
                                 &RandomNumber0);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RandomGenerateFloat(Rng,
                                 (FLOAT) 0.0,
                                 (FLOAT) 1.0,
                                 &RandomNumber1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    //
    // Uniformly sample unit disk, must take 
    // sqrt of r to make samples uniform
    //

    Radius = SqrtFloat(RandomNumber0);
    Theta = IRIS_TWO_PI * RandomNumber1;

    X = Radius * CosineFloat(Theta);
    Y = Radius * SineFloat(Theta);
    Z = SqrtFloat((FLOAT) 1.0 - RandomNumber0);

    Result = VectorCreate(X, Y, Z);

    *RandomVector = IrisToolkitTransformVector(NormalizedNormal, Result);

    return ISTATUS_SUCCESS;
}

#endif // _MATH_IRIS_TOOLKIT_