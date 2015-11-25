/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisadvanced_math.h

Abstract:

    This module includes the Iris Advanced math routines

--*/

#ifndef _MATH_IRIS_ADVANCED_HEADER_
#define _MATH_IRIS_ADVANCED_HEADER_

#include <irisadvanced.h>

//
// Functions
//

SFORCEINLINE
VECTOR3
IrisAdvancedCreateOrthogonalVector(
    _In_ VECTOR3 NormalizedVector
    )
{
    VECTOR3 OrthogonalVector;
    VECTOR_AXIS ShortestAxis;

    ASSERT(VectorValidate(NormalizedVector));

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

    OrthogonalVector = VectorNormalize(OrthogonalVector, NULL, NULL);

    return OrthogonalVector;
}

SFORCEINLINE
VECTOR3
IrisAdvancedTransformVector(
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

    ASSERT(VectorValidate(NormalizedNormal) != FALSE);
    ASSERT(VectorValidate(VectorToTransform) != FALSE);

    OrthogonalVector = IrisAdvancedCreateOrthogonalVector(NormalizedNormal);
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

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IrisAdvancedCosineSampleHemisphere(
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

    ASSERT(VectorValidate(NormalizedNormal) != FALSE);
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

    *RandomVector = IrisAdvancedTransformVector(NormalizedNormal, Result);

    return ISTATUS_SUCCESS;
}

#endif // _MATH_IRIS_ADVANCED_HEADER_