/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    matrix.c

Abstract:

    This module implements the Iris matrix functions.

--*/

#include <irisp.h>

//
// Types
//

struct _INVERTIBLE_MATRIX {
    MATRIX Matrix;
    MATRIX Inverse;
    SIZE_T ReferenceCount;
};

typedef CONST INVERTIBLE_MATRIX *PCINVERTIBLE_MATRIX;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
MatrixPairInitialize(
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33,
    _In_ PINVERTIBLE_MATRIX InvertibleMatrix,
    _Out_ PMATRIX Matrix,
    _Out_ PMATRIX Inverse
    )
{
    ISTATUS Status;
    
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
    ASSERT(InvertibleMatrix != NULL);
    ASSERT(Inverse != NULL);
    ASSERT(Matrix != NULL);
    
    Status = MatrixReferencePairInitialize(M00,
                                           M01,
                                           M02,
                                           M03,
                                           M10,
                                           M11,
                                           M12,
                                           M13,
                                           M20,
                                           M21,
                                           M22,
                                           M23,
                                           M30,
                                           M31,
                                           M32,
                                           M33,
                                           &Matrix->MatrixReference,
                                           &Inverse->MatrixReference);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Matrix->InvertibleMatrix = InvertibleMatrix;
    Inverse->InvertibleMatrix = InvertibleMatrix;
    
    return Status;
}

SFORCEINLINE
VOID
MatrixPairInitializeFromValues(
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33,
    _In_ FLOAT I00,
    _In_ FLOAT I01,
    _In_ FLOAT I02,
    _In_ FLOAT I03,
    _In_ FLOAT I10,
    _In_ FLOAT I11,
    _In_ FLOAT I12,
    _In_ FLOAT I13,
    _In_ FLOAT I20,
    _In_ FLOAT I21,
    _In_ FLOAT I22,
    _In_ FLOAT I23,
    _In_ FLOAT I30,
    _In_ FLOAT I31,
    _In_ FLOAT I32,
    _In_ FLOAT I33,
    _In_ PINVERTIBLE_MATRIX InvertibleMatrix,
    _Out_ PMATRIX Matrix,
    _Out_ PMATRIX Inverse
    )
{
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
    ASSERT(IsFiniteFloat(I00) != FALSE);
    ASSERT(IsFiniteFloat(I01) != FALSE);
    ASSERT(IsFiniteFloat(I02) != FALSE);
    ASSERT(IsFiniteFloat(I03) != FALSE);
    ASSERT(IsFiniteFloat(I10) != FALSE);
    ASSERT(IsFiniteFloat(I11) != FALSE);
    ASSERT(IsFiniteFloat(I12) != FALSE);
    ASSERT(IsFiniteFloat(I13) != FALSE);
    ASSERT(IsFiniteFloat(I20) != FALSE);
    ASSERT(IsFiniteFloat(I21) != FALSE);
    ASSERT(IsFiniteFloat(I22) != FALSE);
    ASSERT(IsFiniteFloat(I23) != FALSE);
    ASSERT(IsFiniteFloat(I30) != FALSE);
    ASSERT(IsFiniteFloat(I31) != FALSE);
    ASSERT(IsFiniteFloat(I32) != FALSE);
    ASSERT(IsFiniteFloat(I33) != FALSE);
    ASSERT(InvertibleMatrix != NULL);
    ASSERT(Inverse != NULL);
    ASSERT(Matrix != NULL);
    
    MatrixReferencePairInitializeFromValues(M00,
                                            M01,
                                            M02,
                                            M03,
                                            M10,
                                            M11,
                                            M12,
                                            M13,
                                            M20,
                                            M21,
                                            M22,
                                            M23,
                                            M30,
                                            M31,
                                            M32,
                                            M33,
                                            I00,
                                            I01,
                                            I02,
                                            I03,
                                            I10,
                                            I11,
                                            I12,
                                            I13,
                                            I20,
                                            I21,
                                            I22,
                                            I23,
                                            I30,
                                            I31,
                                            I32,
                                            I33,
                                            &Matrix->MatrixReference,
                                            &Inverse->MatrixReference);
    
    Matrix->InvertibleMatrix = InvertibleMatrix;
    Inverse->InvertibleMatrix = InvertibleMatrix;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateFromValues(
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33,
    _In_ FLOAT I00,
    _In_ FLOAT I01,
    _In_ FLOAT I02,
    _In_ FLOAT I03,
    _In_ FLOAT I10,
    _In_ FLOAT I11,
    _In_ FLOAT I12,
    _In_ FLOAT I13,
    _In_ FLOAT I20,
    _In_ FLOAT I21,
    _In_ FLOAT I22,
    _In_ FLOAT I23,
    _In_ FLOAT I30,
    _In_ FLOAT I31,
    _In_ FLOAT I32,
    _In_ FLOAT I33,
    _Out_ PMATRIX *Matrix
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;
    
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
    ASSERT(IsFiniteFloat(I00) != FALSE);
    ASSERT(IsFiniteFloat(I01) != FALSE);
    ASSERT(IsFiniteFloat(I02) != FALSE);
    ASSERT(IsFiniteFloat(I03) != FALSE);
    ASSERT(IsFiniteFloat(I10) != FALSE);
    ASSERT(IsFiniteFloat(I11) != FALSE);
    ASSERT(IsFiniteFloat(I12) != FALSE);
    ASSERT(IsFiniteFloat(I13) != FALSE);
    ASSERT(IsFiniteFloat(I20) != FALSE);
    ASSERT(IsFiniteFloat(I21) != FALSE);
    ASSERT(IsFiniteFloat(I22) != FALSE);
    ASSERT(IsFiniteFloat(I23) != FALSE);
    ASSERT(IsFiniteFloat(I30) != FALSE);
    ASSERT(IsFiniteFloat(I31) != FALSE);
    ASSERT(IsFiniteFloat(I32) != FALSE);
    ASSERT(IsFiniteFloat(I33) != FALSE);
    ASSERT(Matrix != NULL);

    InvertibleMatrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (InvertibleMatrix == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    MatrixPairInitializeFromValues(M00,
                                   M01,
                                   M02,
                                   M03,
                                   M10,
                                   M11,
                                   M12,
                                   M13,
                                   M20,
                                   M21,
                                   M22,
                                   M23,
                                   M30,
                                   M31,
                                   M32,
                                   M33,
                                   I00,
                                   I01,
                                   I02,
                                   I03,
                                   I10,
                                   I11,
                                   I12,
                                   I13,
                                   I20,
                                   I21,
                                   I22,
                                   I23,
                                   I30,
                                   I31,
                                   I32,
                                   I33,
                                   InvertibleMatrix,
                                   &InvertibleMatrix->Matrix,
                                   &InvertibleMatrix->Inverse);

    InvertibleMatrix->ReferenceCount = 1;

    *Matrix = &InvertibleMatrix->Matrix;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
MatrixAllocateRotationFromVector(
    _In_ FLOAT Theta,
    _In_ VECTOR3 Axis,
    _Out_ PMATRIX *Matrix
    )
{
    FLOAT Cos;
    FLOAT Ic;
    FLOAT M00;
    FLOAT M01;
    FLOAT M02;
    FLOAT M10;
    FLOAT M11;
    FLOAT M12;
    FLOAT M20;
    FLOAT M21;
    FLOAT M22;
    VECTOR3 NormalizedAxis;
    FLOAT Sin;
    ISTATUS Status;

    ASSERT(IsFiniteFloat(Theta) != FALSE);
    ASSERT(VectorValidate(Axis) != FALSE);
    ASSERT(Matrix != NULL);

    NormalizedAxis = VectorNormalize(Axis, NULL, NULL);

    Sin = SineFloat(Theta * IRIS_PI / (FLOAT) 180.0);
    Cos = CosineFloat(Theta * IRIS_PI / (FLOAT) 180.0);
    Ic = 1.0f - Cos;

    M00 = NormalizedAxis.X * NormalizedAxis.X * Ic + Cos;
    M01 = NormalizedAxis.X * NormalizedAxis.Y * Ic - NormalizedAxis.Z * Sin;
    M02 = NormalizedAxis.X * NormalizedAxis.Z * Ic + NormalizedAxis.Y * Sin;

    M10 = NormalizedAxis.Y * NormalizedAxis.X * Ic + NormalizedAxis.Z * Sin;
    M11 = NormalizedAxis.Y * NormalizedAxis.Y * Ic + Cos;
    M12 = NormalizedAxis.Y * NormalizedAxis.X * Ic - NormalizedAxis.X * Sin;
    
    M20 = NormalizedAxis.Z * NormalizedAxis.X * Ic - NormalizedAxis.Y * Sin;
    M21 = NormalizedAxis.Z * NormalizedAxis.Y * Ic + NormalizedAxis.X * Sin;
    M22 = NormalizedAxis.Z * NormalizedAxis.Z * Ic + Cos;

    Status = MatrixAllocate(M00, 
                            M01, 
                            M02, 
                            (FLOAT) 0.0, 
                            M10,
                            M11,
                            M12,
                            (FLOAT) 0.0,
                            M20,
                            M21,
                            M22,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 1.0,
                            Matrix);

    return Status;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocate(
    _In_ FLOAT M00,
    _In_ FLOAT M01,
    _In_ FLOAT M02,
    _In_ FLOAT M03,
    _In_ FLOAT M10,
    _In_ FLOAT M11,
    _In_ FLOAT M12,
    _In_ FLOAT M13,
    _In_ FLOAT M20,
    _In_ FLOAT M21,
    _In_ FLOAT M22,
    _In_ FLOAT M23,
    _In_ FLOAT M30,
    _In_ FLOAT M31,
    _In_ FLOAT M32,
    _In_ FLOAT M33,
    _Out_ PMATRIX *Matrix
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;
    ISTATUS Status;

    if (IsFiniteFloat(M00) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(M01) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(M02) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsFiniteFloat(M03) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (IsFiniteFloat(M10) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsFiniteFloat(M11) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (IsFiniteFloat(M12) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (IsFiniteFloat(M13) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (IsFiniteFloat(M20) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (IsFiniteFloat(M21) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    if (IsFiniteFloat(M22) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_10;
    }

    if (IsFiniteFloat(M23) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_11;
    }

    if (IsFiniteFloat(M30) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_12;
    }

    if (IsFiniteFloat(M31) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_13;
    }

    if (IsFiniteFloat(M32) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_14;
    }

    if (IsFiniteFloat(M33) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_15;
    }

    if (Matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_16;
    }

    InvertibleMatrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (InvertibleMatrix == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = MatrixPairInitialize(M00,
                                  M01,
                                  M02,
                                  M03,
                                  M10,
                                  M11,
                                  M12,
                                  M13,
                                  M20,
                                  M21,
                                  M22,
                                  M23,
                                  M30,
                                  M31,
                                  M32,
                                  M33,
                                  InvertibleMatrix,
                                  &InvertibleMatrix->Matrix,
                                  &InvertibleMatrix->Inverse);

    if (Status != ISTATUS_SUCCESS)
    {
        free(InvertibleMatrix);
        return Status;
    }

    InvertibleMatrix->ReferenceCount = 1;

    *Matrix = &InvertibleMatrix->Matrix;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateTranslation(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z,
    _Out_ PMATRIX *Matrix
    )
{
    ISTATUS Status;

    if (IsFiniteFloat(X) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(Y) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Z) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = MatrixAllocateFromValues((FLOAT) 1.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      X,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      (FLOAT) 0.0,
                                      Y,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      Z,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      (FLOAT) 1.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      -X,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      (FLOAT) 0.0,
                                      -Y,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      -Z,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      Matrix);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateScalar(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z,
    _Out_ PMATRIX *Matrix
    )
{
    ISTATUS Status;

    if (IsFiniteFloat(X) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(Y) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Z) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = MatrixAllocateFromValues(X,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      Y,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      Z,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      (FLOAT) 1.0 / X,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0 / Y,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0 / Z,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 0.0,
                                      (FLOAT) 1.0,
                                      Matrix);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateRotation(
    _In_ FLOAT Theta,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z,
    _Out_ PMATRIX *Matrix
    )
{
    VECTOR3 Axis;
    ISTATUS Status;

    if (IsFiniteFloat(Theta) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(X) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Y) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsFiniteFloat(Z) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsNotZeroFloat(X) == FALSE &&
        IsNotZeroFloat(Y) == FALSE &&
        IsNotZeroFloat(Z) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    Axis = VectorCreate(X, Y, Z);

    Status = MatrixAllocateRotationFromVector(Theta, Axis, Matrix);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateOrthographic(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far,
    _Out_ PMATRIX *Matrix
    )
{
    ISTATUS Status;
    FLOAT Sx;
    FLOAT Sy;
    FLOAT Sz;
    FLOAT Tx;
    FLOAT Ty;
    FLOAT Tz;

    if (IsFiniteFloat(Left) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(Right) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Bottom) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsFiniteFloat(Top) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (IsFiniteFloat(Near) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsFiniteFloat(Far) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Left == Right)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (Bottom == Top)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (Near == Far)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    Tx = -(Right + Left) / (Right - Left);
    Ty = -(Top + Bottom) / (Top - Bottom);
    Tz = -(Far + Near) / (Far - Near);

    Sx = (FLOAT) 2.0 / (Right - Left);
    Sy = (FLOAT) 2.0 / (Top - Bottom);
    Sz = (FLOAT) -2.0 / (Far - Near);

    Status = MatrixAllocate(Sx,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            Tx,
                            (FLOAT) 0.0,
                            Sy,
                            (FLOAT) 0.0,
                            Ty,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            Sz,
                            Tz,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 1.0,
                            Matrix);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateFrustum(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far,
    _Out_ PMATRIX *Matrix
    )
{
    FLOAT A;
    FLOAT B;
    FLOAT C;
    FLOAT D;
    ISTATUS Status;
    FLOAT Sx;
    FLOAT Sy;

    if (IsFiniteFloat(Left) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(Right) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(Bottom) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsFiniteFloat(Top) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (IsFiniteFloat(Near) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Near) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsFiniteFloat(Far) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Far) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Left == Right)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (Bottom == Top)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (Near == Far)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    Sx = ((FLOAT) 2.0 * Near) / (Right - Left);
    Sy = ((FLOAT) 2.0 * Near) / (Top - Bottom);

    A = (Right + Left) / (Right - Left);
    B = (Top + Bottom) / (Top - Bottom);

    C = -(Far + Near) / (Far - Near);
    D = (FLOAT) -2.0 * Far * Near / (Far - Near);

    Status = MatrixAllocate(Sx,
                            (FLOAT) 0.0,
                            A,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            Sy,
                            B,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            C,
                            D,
                            (FLOAT) 0.0,
                            (FLOAT) 0.0,
                            (FLOAT) -1.0,
                            (FLOAT) 0.0,
                            Matrix);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MatrixAllocateProduct(
    _In_opt_ PMATRIX Multiplicand0,
    _In_opt_ PMATRIX Multiplicand1,
    _Out_ PMATRIX *Result
    )
{
    FLOAT M00;
    FLOAT M01;
    FLOAT M02;
    FLOAT M03;
    FLOAT M10;
    FLOAT M11;
    FLOAT M12;
    FLOAT M13;
    FLOAT M20;
    FLOAT M21;
    FLOAT M22;
    FLOAT M23;
    FLOAT M30;
    FLOAT M31;
    FLOAT M32;
    FLOAT M33;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Multiplicand0 == NULL && 
        Multiplicand1 == NULL)
    {
        *Result = NULL;
        return ISTATUS_SUCCESS;
    }

    if (Multiplicand0 == NULL)
    {
        MatrixReference(Multiplicand1);
        *Result = Multiplicand1;
        return ISTATUS_SUCCESS;
    }

    if (Multiplicand1 == NULL)
    {
        MatrixReference(Multiplicand0);
        *Result = Multiplicand0;
        return ISTATUS_SUCCESS;
    }

    M00 = Multiplicand0->MatrixReference.M[0][0] * Multiplicand1->MatrixReference.M[0][0] + 
          Multiplicand0->MatrixReference.M[0][1] * Multiplicand1->MatrixReference.M[1][0] +
          Multiplicand0->MatrixReference.M[0][2] * Multiplicand1->MatrixReference.M[2][0] +
          Multiplicand0->MatrixReference.M[0][3] * Multiplicand1->MatrixReference.M[3][0];

    M01 = Multiplicand0->MatrixReference.M[0][0] * Multiplicand1->MatrixReference.M[0][1] + 
          Multiplicand0->MatrixReference.M[0][1] * Multiplicand1->MatrixReference.M[1][1] +
          Multiplicand0->MatrixReference.M[0][2] * Multiplicand1->MatrixReference.M[2][1] +
          Multiplicand0->MatrixReference.M[0][3] * Multiplicand1->MatrixReference.M[3][1];

    M02 = Multiplicand0->MatrixReference.M[0][0] * Multiplicand1->MatrixReference.M[0][2] + 
          Multiplicand0->MatrixReference.M[0][1] * Multiplicand1->MatrixReference.M[1][2] +
          Multiplicand0->MatrixReference.M[0][2] * Multiplicand1->MatrixReference.M[2][2] +
          Multiplicand0->MatrixReference.M[0][3] * Multiplicand1->MatrixReference.M[3][2];

    M03 = Multiplicand0->MatrixReference.M[0][0] * Multiplicand1->MatrixReference.M[0][3] + 
          Multiplicand0->MatrixReference.M[0][1] * Multiplicand1->MatrixReference.M[1][3] +
          Multiplicand0->MatrixReference.M[0][2] * Multiplicand1->MatrixReference.M[2][3] +
          Multiplicand0->MatrixReference.M[0][3] * Multiplicand1->MatrixReference.M[3][3];

    M10 = Multiplicand0->MatrixReference.M[1][0] * Multiplicand1->MatrixReference.M[0][0] + 
          Multiplicand0->MatrixReference.M[1][1] * Multiplicand1->MatrixReference.M[1][0] +
          Multiplicand0->MatrixReference.M[1][2] * Multiplicand1->MatrixReference.M[2][0] +
          Multiplicand0->MatrixReference.M[1][3] * Multiplicand1->MatrixReference.M[3][0];

    M11 = Multiplicand0->MatrixReference.M[1][0] * Multiplicand1->MatrixReference.M[0][1] + 
          Multiplicand0->MatrixReference.M[1][1] * Multiplicand1->MatrixReference.M[1][1] +
          Multiplicand0->MatrixReference.M[1][2] * Multiplicand1->MatrixReference.M[2][1] +
          Multiplicand0->MatrixReference.M[1][3] * Multiplicand1->MatrixReference.M[3][1];

    M12 = Multiplicand0->MatrixReference.M[1][0] * Multiplicand1->MatrixReference.M[0][2] + 
          Multiplicand0->MatrixReference.M[1][1] * Multiplicand1->MatrixReference.M[1][2] +
          Multiplicand0->MatrixReference.M[1][2] * Multiplicand1->MatrixReference.M[2][2] +
          Multiplicand0->MatrixReference.M[1][3] * Multiplicand1->MatrixReference.M[3][2];

    M13 = Multiplicand0->MatrixReference.M[1][0] * Multiplicand1->MatrixReference.M[0][3] + 
          Multiplicand0->MatrixReference.M[1][1] * Multiplicand1->MatrixReference.M[1][3] +
          Multiplicand0->MatrixReference.M[1][2] * Multiplicand1->MatrixReference.M[2][3] +
          Multiplicand0->MatrixReference.M[1][3] * Multiplicand1->MatrixReference.M[3][3];

    M20 = Multiplicand0->MatrixReference.M[2][0] * Multiplicand1->MatrixReference.M[0][0] + 
          Multiplicand0->MatrixReference.M[2][1] * Multiplicand1->MatrixReference.M[1][0] +
          Multiplicand0->MatrixReference.M[2][2] * Multiplicand1->MatrixReference.M[2][0] +
          Multiplicand0->MatrixReference.M[2][3] * Multiplicand1->MatrixReference.M[3][0];

    M21 = Multiplicand0->MatrixReference.M[2][0] * Multiplicand1->MatrixReference.M[0][1] + 
          Multiplicand0->MatrixReference.M[2][1] * Multiplicand1->MatrixReference.M[1][1] +
          Multiplicand0->MatrixReference.M[2][2] * Multiplicand1->MatrixReference.M[2][1] +
          Multiplicand0->MatrixReference.M[2][3] * Multiplicand1->MatrixReference.M[3][1];

    M22 = Multiplicand0->MatrixReference.M[2][0] * Multiplicand1->MatrixReference.M[0][2] + 
          Multiplicand0->MatrixReference.M[2][1] * Multiplicand1->MatrixReference.M[1][2] +
          Multiplicand0->MatrixReference.M[2][2] * Multiplicand1->MatrixReference.M[2][2] +
          Multiplicand0->MatrixReference.M[2][3] * Multiplicand1->MatrixReference.M[3][2];

    M23 = Multiplicand0->MatrixReference.M[2][0] * Multiplicand1->MatrixReference.M[0][3] + 
          Multiplicand0->MatrixReference.M[2][1] * Multiplicand1->MatrixReference.M[1][3] +
          Multiplicand0->MatrixReference.M[2][2] * Multiplicand1->MatrixReference.M[2][3] +
          Multiplicand0->MatrixReference.M[2][3] * Multiplicand1->MatrixReference.M[3][3];

    M30 = Multiplicand0->MatrixReference.M[3][0] * Multiplicand1->MatrixReference.M[0][0] + 
          Multiplicand0->MatrixReference.M[3][1] * Multiplicand1->MatrixReference.M[1][0] +
          Multiplicand0->MatrixReference.M[3][2] * Multiplicand1->MatrixReference.M[2][0] +
          Multiplicand0->MatrixReference.M[3][3] * Multiplicand1->MatrixReference.M[3][0];

    M31 = Multiplicand0->MatrixReference.M[3][0] * Multiplicand1->MatrixReference.M[0][1] + 
          Multiplicand0->MatrixReference.M[3][1] * Multiplicand1->MatrixReference.M[1][1] +
          Multiplicand0->MatrixReference.M[3][2] * Multiplicand1->MatrixReference.M[2][1] +
          Multiplicand0->MatrixReference.M[3][3] * Multiplicand1->MatrixReference.M[3][1];

    M32 = Multiplicand0->MatrixReference.M[3][0] * Multiplicand1->MatrixReference.M[0][2] + 
          Multiplicand0->MatrixReference.M[3][1] * Multiplicand1->MatrixReference.M[1][2] +
          Multiplicand0->MatrixReference.M[3][2] * Multiplicand1->MatrixReference.M[2][2] +
          Multiplicand0->MatrixReference.M[3][3] * Multiplicand1->MatrixReference.M[3][2];

    M33 = Multiplicand0->MatrixReference.M[3][0] * Multiplicand1->MatrixReference.M[0][3] + 
          Multiplicand0->MatrixReference.M[3][1] * Multiplicand1->MatrixReference.M[1][3] +
          Multiplicand0->MatrixReference.M[3][2] * Multiplicand1->MatrixReference.M[2][3] +
          Multiplicand0->MatrixReference.M[3][3] * Multiplicand1->MatrixReference.M[3][3];

    Status = MatrixAllocate(M00,
                            M01,
                            M02,
                            M03,
                            M10,
                            M11,
                            M12,
                            M13,
                            M20,
                            M21,
                            M22,
                            M23,
                            M30,
                            M31,
                            M32,
                            M33,
                            Result);

    return Status;
}

_Ret_opt_
PMATRIX
MatrixGetInverse(
    _In_opt_ PMATRIX Matrix
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;
    PMATRIX Output;
    
    if (Matrix == NULL)
    {
        return NULL;
    }
    
    InvertibleMatrix = Matrix->InvertibleMatrix;

    InvertibleMatrix->ReferenceCount += 1;
    
    if (Matrix == &InvertibleMatrix->Matrix)
    {
        Output = &InvertibleMatrix->Inverse;
    }
    else
    {
        Output = &InvertibleMatrix->Matrix;
    }

    return Output;
}

ISTATUS
MatrixReadContents(
    _In_ PCMATRIX Matrix,
    _Out_writes_(4) FLOAT Contents[4][4]
    )
{
    ISTATUS Status;
    
    if (Matrix == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = MatrixReferenceReadContents(&Matrix->MatrixReference, 
                                         Contents);

    return Status;
}

VOID
MatrixReference(
    _In_opt_ PMATRIX Matrix
    )
{
    if (Matrix == NULL)
    {
        return;
    }

    Matrix->InvertibleMatrix->ReferenceCount += 1;
}

VOID
MatrixDereference(
    _In_opt_ _Post_invalid_ PMATRIX Matrix
    )
{
    if (Matrix == NULL)
    {
        return;
    }

    Matrix->InvertibleMatrix->ReferenceCount -= 1;

    if (Matrix->InvertibleMatrix->ReferenceCount == 0)
    {
        free(Matrix->InvertibleMatrix);
    }
}