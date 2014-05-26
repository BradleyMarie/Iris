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

#define IRIS_PI ((FLOAT) 3.1415926535897932384626433832795)
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
VOID
IrisToolkitGenerateOrthogonalVector(
    _In_ PCVECTOR3 NormalizedVector,
    _Out_ PVECTOR3 OrthogonalVector
    )
{
    VECTOR_AXIS ShortestAxis;

    ShortestAxis = VectorDiminishedAxis(NormalizedVector);

    switch (ShortestAxis)
    {
        case VECTOR_X_AXIS:
            VectorInitialize(OrthogonalVector, 
                             (FLOAT) 1.0, 
                             (FLOAT) 0.0,
                             (FLOAT) 0.0);
            break;
        case VECTOR_Y_AXIS:
            VectorInitialize(OrthogonalVector,
                             (FLOAT) 0.0,
                             (FLOAT) 1.0,
                             (FLOAT) 0.0);
            break;
        default: // VECTOR_Z_AXIS
            VectorInitialize(OrthogonalVector,
                             (FLOAT) 0.0,
                             (FLOAT) 0.0,
                             (FLOAT) 1.0);
            break;
    }

    VectorCrossProduct(NormalizedVector, OrthogonalVector, OrthogonalVector);
    VectorNormalize(OrthogonalVector, OrthogonalVector);
}

SFORCEINLINE
VOID
IrisToolkitTransformVector(
    _In_ PCVECTOR3 NormalizedNormal,
    _In_ PCVECTOR3 VectorToTransform,
    _Out_ PVECTOR3 TransformedVector
    )
{
    VECTOR3 OrthogonalVector;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    IrisToolkitGenerateOrthogonalVector(NormalizedNormal, &OrthogonalVector);
    VectorCrossProduct(NormalizedNormal, &OrthogonalVector, TransformedVector);

    X = OrthogonalVector.X * VectorToTransform->X + 
        TransformedVector->X * VectorToTransform->Y + 
        NormalizedNormal->X * VectorToTransform->Z;

    Y = OrthogonalVector.Y * VectorToTransform->X + 
        TransformedVector->Y * VectorToTransform->Y + 
        NormalizedNormal->Y * VectorToTransform->Z;

    Z = OrthogonalVector.Z * VectorToTransform->X + 
        TransformedVector->Z * VectorToTransform->Y + 
        NormalizedNormal->Z * VectorToTransform->Z;

    VectorInitialize(TransformedVector, X, Y, Z);
}

SFORCEINLINE
VOID
IrisToolkitCosineSampleHemisphere(
    _In_ PCVECTOR3 NormalizedNormal,
    _Inout_ PRANDOM Rng,
    _Out_ PVECTOR3 RandomVector
    )
{
    FLOAT RandomNumber0;
    FLOAT RandomNumber1;
    FLOAT Radius;
    FLOAT Theta;
    FLOAT X;
    FLOAT Y;
    FLOAT Z;

    RandomNumber0 = RandomGenerateFloat(Rng, (FLOAT) 0.0, (FLOAT) 1.0);
    RandomNumber1 = RandomGenerateFloat(Rng, (FLOAT) 0.0, (FLOAT) 1.0);

    //
    // Uniformly sample unit disk, must take 
    // sqrt of r to make samples uniform
    //

    Radius = SqrtFloat(RandomNumber0);
    Theta = IRIS_TWO_PI * RandomNumber1;

    X = Radius * CosineFloat(Theta);
    Y = Radius * SineFloat(Theta);
    Z = SqrtFloat((FLOAT) 1.0 - RandomNumber0);

    VectorInitialize(RandomVector, X, Y, Z);

    IrisToolkitTransformVector(NormalizedNormal, RandomVector, RandomVector);
}

#endif // _MATH_IRIS_TOOLKIT_