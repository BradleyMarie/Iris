/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    matrix.c

Abstract:

    This module implements the Iris matrix functions.

--*/

#include <iris.h>

STATIC
VOID
MatrixpDivideRow(
    __inout PMATRIX Matrix,
    __in_range(0, 3) SIZE_T Row,
    __in FLOAT Divisor
    )
{
    FLOAT Scalar;

    ASSERT(Matrix != NULL);
    ASSERT(0 <= Row);
    ASSERT(Row <= 3);
    ASSERT(IsNormalFloat(Divisor));
    ASSERT(IsFiniteFloat(Divisor));
    ASSERT(Divisor != (FLOAT)0.0);

    Scalar = (FLOAT)1.0 / Divisor;

    Matrix->M[Row][0] *= Divisor;
    Matrix->M[Row][1] *= Divisor;
    Matrix->M[Row][2] *= Divisor;
    Matrix->M[Row][3] *= Divisor;
}

STATIC
VOID
MatrixpScaledSubtractRow(
    __inout PMATRIX Matrix,
    __in_range(0, 3) SIZE_T ConstantRow,
    __in_range(0, 3) SIZE_T ModifiedRow,
    __in FLOAT Scalar
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(0 <= ConstantRow);
    ASSERT(ConstantRow <= 3);
    ASSERT(0 <= ModifiedRow);
    ASSERT(ModifiedRow <= 3);
    ASSERT(IsNormalFloat(Scalar));
    ASSERT(IsFiniteFloat(Scalar));

    Matrix->M[ModifiedRow][0] -= Matrix->M[ConstantRow][0] * Scalar;
    Matrix->M[ModifiedRow][1] -= Matrix->M[ConstantRow][1] * Scalar;
    Matrix->M[ModifiedRow][2] -= Matrix->M[ConstantRow][2] * Scalar;
    Matrix->M[ModifiedRow][3] -= Matrix->M[ConstantRow][3] * Scalar;
}

STATIC
VOID
MatrixpSwapRows(
    __inout PMATRIX Matrix,
    __in_range(0, 3) SIZE_T Row0,
    __in_range(0, 3) SIZE_T Row1
    )
{
    SIZE_T Index;
    FLOAT Temp;

    ASSERT(Matrix != NULL);
    ASSERT(0 <= Row0);
    ASSERT(Row0 <= 3);
    ASSERT(0 <= Row1);
    ASSERT(Row1 <= 3);

    for (Index = 0; Index < 4; Index++)
    {
        Temp = Matrix->M[Row0][Index];
        Matrix->M[Row0][Index] = Matrix->M[Row1][Index];
        Matrix->M[Row1][Index] = Temp;
    }
}

STATIC
VOID
MatrixpInitialize(
    __out PMATRIX Matrix,
    __in FLOAT M00,
    __in FLOAT M01,
    __in FLOAT M02,
    __in FLOAT M03,
    __in FLOAT M10,
    __in FLOAT M11,
    __in FLOAT M12,
    __in FLOAT M13,
    __in FLOAT M20,
    __in FLOAT M21,
    __in FLOAT M22,
    __in FLOAT M23,
    __in FLOAT M30,
    __in FLOAT M31,
    __in FLOAT M32,
    __in FLOAT M33,
    __in PMATRIX Inverse
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(M00));
    ASSERT(IsFiniteFloat(M00));
    ASSERT(IsNormalFloat(M01));
    ASSERT(IsFiniteFloat(M01));
    ASSERT(IsNormalFloat(M02));
    ASSERT(IsFiniteFloat(M02));
    ASSERT(IsNormalFloat(M03));
    ASSERT(IsFiniteFloat(M03));
    ASSERT(IsNormalFloat(M10));
    ASSERT(IsFiniteFloat(M10));
    ASSERT(IsNormalFloat(M11));
    ASSERT(IsFiniteFloat(M11));
    ASSERT(IsNormalFloat(M12));
    ASSERT(IsFiniteFloat(M12));
    ASSERT(IsNormalFloat(M13));
    ASSERT(IsFiniteFloat(M13));
    ASSERT(IsNormalFloat(M20));
    ASSERT(IsFiniteFloat(M20));
    ASSERT(IsNormalFloat(M21));
    ASSERT(IsFiniteFloat(M21));
    ASSERT(IsNormalFloat(M22));
    ASSERT(IsFiniteFloat(M22));
    ASSERT(IsNormalFloat(M23));
    ASSERT(IsFiniteFloat(M23));
    ASSERT(IsNormalFloat(M30));
    ASSERT(IsFiniteFloat(M30));
    ASSERT(IsNormalFloat(M31));
    ASSERT(IsFiniteFloat(M31));
    ASSERT(IsNormalFloat(M32));
    ASSERT(IsFiniteFloat(M32));
    ASSERT(IsNormalFloat(M33));
    ASSERT(IsFiniteFloat(M33));
    ASSERT(Inverse != NULL);

    Matrix->M[0][0] = M00;
    Matrix->M[0][1] = M01;
    Matrix->M[0][2] = M02;
    Matrix->M[0][3] = M03;
    Matrix->M[1][0] = M10;
    Matrix->M[1][1] = M11;
    Matrix->M[1][2] = M12;
    Matrix->M[1][3] = M13;
    Matrix->M[2][0] = M20;
    Matrix->M[2][1] = M21;
    Matrix->M[2][2] = M22;
    Matrix->M[2][3] = M23;
    Matrix->M[3][0] = M30;
    Matrix->M[3][1] = M31;
    Matrix->M[3][2] = M32;
    Matrix->M[3][3] = M33;

    Matrix->Inverse = Inverse;
}

STATIC
VOID
MatrixpInitializeIdentity(
    __out PMATRIX Matrix
    )
{
    ASSERT(Matrix != NULL);

    MatrixpInitialize(Matrix, 
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      Matrix);
}

__success(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
MatrixpInvert(
    __inout PMATRIX Matrix,
    __out PMATRIX Inverse
    )
{
    MATRIX TemporaryMatrix;
    SIZE_T Index;
    SIZE_T Lead;
    SIZE_T R;

    ASSERT(Matrix != NULL);
    ASSERT(Inverse != NULL);

    MatrixpInitializeIdentity(Inverse);

    Lead = 0;

    for (R = 0; R < 4; R++)
    {
        if (R <= Lead)
        {
            break;
        }

        Index = R;

        while (TemporaryMatrix.M[Index][Lead] == (FLOAT)0.0)
        {
            Index++;

            if (Index == 4)
            {
                Index = R;
                Lead++;

                if (Lead == 4)
                {
                    return ISTATUS_ARITHMETIC_ERROR;
                }
            }
        }

        if (Index != R)
        {
            MatrixpSwapRows(&TemporaryMatrix, Index, R);
            MatrixpSwapRows(Inverse, Index, R);
        }

        MatrixpDivideRow(&TemporaryMatrix, R, TemporaryMatrix.M[R][Lead]);
        MatrixpDivideRow(Inverse, R, TemporaryMatrix.M[R][Lead]);

        for (Index = 0; Index < 4; Index++)
        {
            if (Index != R)
            {
                MatrixpScaledSubtractRow(&TemporaryMatrix,
                                         R,
                                         Index,
                                         TemporaryMatrix.M[Index][Lead]);

                MatrixpScaledSubtractRow(Inverse, 
                                         R, 
                                         Index,
                                         TemporaryMatrix.M[Index][Lead]);
            }
        }

        Lead++;
    }

    return ISTATUS_SUCCESS;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitialize(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT M00,
    __in FLOAT M01,
    __in FLOAT M02,
    __in FLOAT M03,
    __in FLOAT M10,
    __in FLOAT M11,
    __in FLOAT M12,
    __in FLOAT M13,
    __in FLOAT M20,
    __in FLOAT M21,
    __in FLOAT M22,
    __in FLOAT M23,
    __in FLOAT M30,
    __in FLOAT M31,
    __in FLOAT M32,
    __in FLOAT M33
    )
{
    MATRIX TemporaryInverse;
    MATRIX TemporaryMatrix;
    ISTATUS Status;

    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(M00));
    ASSERT(IsFiniteFloat(M00));
    ASSERT(IsNormalFloat(M01));
    ASSERT(IsFiniteFloat(M01));
    ASSERT(IsNormalFloat(M02));
    ASSERT(IsFiniteFloat(M02));
    ASSERT(IsNormalFloat(M03));
    ASSERT(IsFiniteFloat(M03));
    ASSERT(IsNormalFloat(M10));
    ASSERT(IsFiniteFloat(M10));
    ASSERT(IsNormalFloat(M11));
    ASSERT(IsFiniteFloat(M11));
    ASSERT(IsNormalFloat(M12));
    ASSERT(IsFiniteFloat(M12));
    ASSERT(IsNormalFloat(M13));
    ASSERT(IsFiniteFloat(M13));
    ASSERT(IsNormalFloat(M20));
    ASSERT(IsFiniteFloat(M20));
    ASSERT(IsNormalFloat(M21));
    ASSERT(IsFiniteFloat(M21));
    ASSERT(IsNormalFloat(M22));
    ASSERT(IsFiniteFloat(M22));
    ASSERT(IsNormalFloat(M23));
    ASSERT(IsFiniteFloat(M23));
    ASSERT(IsNormalFloat(M30));
    ASSERT(IsFiniteFloat(M30));
    ASSERT(IsNormalFloat(M31));
    ASSERT(IsFiniteFloat(M31));
    ASSERT(IsNormalFloat(M32));
    ASSERT(IsFiniteFloat(M32));
    ASSERT(IsNormalFloat(M33));
    ASSERT(IsFiniteFloat(M33));

    MatrixpInitialize(&TemporaryMatrix,
                      M00,
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
                      NULL);

    Status = MatrixpInvert(&TemporaryMatrix, &TemporaryInverse);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Matrix->Inverse = TemporaryInverse;
    Matrix->Matrix = TemporaryMatrix;

    return ISTATUS_SUCCESS;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeIdentity(
    __out PINVERTIBLE_MATRIX Matrix
    )
{
    ASSERT(Matrix != NULL);

    MatrixpInitializeIdentity(&Matrix->Matrix);
    MatrixpInitializeIdentity(&Matrix->Inverse);

    return ISTATUS_SUCCESS;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeTranslation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));

    MatrixpInitialize(&Matrix->Matrix, 
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      X,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      Y,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      Z,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      &Matrix->Inverse);

    MatrixpInitialize(&Matrix->Inverse, 
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      -X,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      (FLOAT)0.0,
                      -Y,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      -Z,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      &Matrix->Matrix);

    return ISTATUS_SUCCESS;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeScalar(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));

    if (X == (FLOAT)0.0 || Y == (FLOAT)0.0 || Z == (FLOAT)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    MatrixpInitialize(&Matrix->Matrix, 
                      X,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      Y,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      Z,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      &Matrix->Inverse);

    MatrixpInitialize(&Matrix->Matrix, 
                      (FLOAT)1.0 / X,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0 / Y,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0 / Z,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)0.0,
                      (FLOAT)1.0,
                      &Matrix->Inverse);

    return ISTATUS_SUCCESS;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixpInitializeRotation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Theta,
    __in PVECTOR3 Axis
    )
{
    VECTOR3 NormalizedAxis;
    ISTATUS Status;
    FLOAT Sin;
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

    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(Theta));
    ASSERT(IsFiniteFloat(Theta));
    ASSERT(Axis != NULL);

    if (VectorLength(Axis) == (FLOAT)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    VectorNormalize(Axis, &NormalizedAxis);

    Sin = SinFloat(Theta * PI_FLOAT / (FLOAT)180.0);
    Cos = CosFloat(Theta * PI_FLOAT / (FLOAT)180.0);
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

    Status = MatrixInitialize(Matrix, 
                              M00, 
                              M01, 
                              M02, 
                              (FLOAT)0.0, 
                              M10,
                              M11,
                              M12,
                              (FLOAT)0.0,
                              M20,
                              M21,
                              M22,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)1.0);

    return Status;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeRotation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Theta,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    )
{
    VECTOR3 Axis;

    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(Theta));
    ASSERT(IsFiniteFloat(Theta));
    ASSERT(IsNormalFloat(X));
    ASSERT(IsFiniteFloat(X));
    ASSERT(IsNormalFloat(Y));
    ASSERT(IsFiniteFloat(Y));
    ASSERT(IsNormalFloat(Z));
    ASSERT(IsFiniteFloat(Z));

    VectorInitialize(&Axis, X, Y, Z);

    return MatrixpInitializeRotation(Matrix, Theta, &Axis);
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeFrustum(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Left,
    __in FLOAT Right,
    __in FLOAT Bottom,
    __in FLOAT Top,
    __in FLOAT Near,
    __in FLOAT Far
    )
{
    ISTATUS Status;
    FLOAT Tx;
    FLOAT Ty;
    FLOAT Tz;
    FLOAT Sx;
    FLOAT Sy;
    FLOAT Sz;

    ASSERT(Matrix != NULL);

    if (Left == Right || Bottom == Top || Near == Far)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Tx = -(Right + Left) / (Right - Left);
    Ty = -(Top + Bottom) / (Top - Bottom);
    Tz = -(Far + Near) / (Far - Near);

    Sx = (FLOAT)2.0 / (Right - Left);
    Sy = (FLOAT)2.0 / (Top - Bottom);
    Sz = (FLOAT)-2.0 / (Far - Near);

    Status = MatrixInitialize(Matrix,
                              Sx,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              Tx,
                              (FLOAT)0.0,
                              Sy,
                              (FLOAT)0.0,
                              Ty,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              Sz,
                              Tz,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)1.0);

    return Status;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeOrothographic(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Left,
    __in FLOAT Right,
    __in FLOAT Bottom,
    __in FLOAT Top,
    __in FLOAT Near,
    __in FLOAT Far
    )
{
    ISTATUS Status;
    FLOAT Sx;
    FLOAT Sy;
    FLOAT A;
    FLOAT B;
    FLOAT C;
    FLOAT D;

    ASSERT(Matrix != NULL);

    if (Left == Right || Bottom == Top || Near == Far)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Sx = ((FLOAT)2.0 * Near) / (Right - Left);
    Sy = ((FLOAT)2.0 * Near) / (Top - Bottom);

    A = (Right + Left) / (Right - Left);
    B = (Top + Bottom) / (Top - Bottom);

    C = -(Far + Near) / (Far - Near);
    D = (FLOAT)-2.0 * Far * Near / (Far - Near);

    Status = MatrixInitialize(Matrix,
                              Sx,
                              (FLOAT)0.0,
                              A,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              Sy,
                              B,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              C,
                              D,
                              (FLOAT)0.0,
                              (FLOAT)0.0,
                              (FLOAT)-1.0,
                              (FLOAT)0.0);

    return Status;
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixMultiply(
    __in PINVERTIBLE_MATRIX Multiplicand0,
    __in PINVERTIBLE_MATRIX Multiplicand1,
    __out PINVERTIBLE_MATRIX Product
    )
{
    ISTATUS Status;
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

    ASSERT(Multiplicand0 != NULL);
    ASSERT(Multiplicand1 != NULL);
    ASSERT(Product != NULL);

    M00 = Multiplicand0->Matrix.M[0][0] * Multiplicand1->Matrix.M[0][0] + 
          Multiplicand0->Matrix.M[0][1] * Multiplicand1->Matrix.M[1][0] +
          Multiplicand0->Matrix.M[0][2] * Multiplicand1->Matrix.M[2][0] +
          Multiplicand0->Matrix.M[0][3] * Multiplicand1->Matrix.M[3][0];

    M01 = Multiplicand0->Matrix.M[0][0] * Multiplicand1->Matrix.M[0][1] + 
          Multiplicand0->Matrix.M[0][1] * Multiplicand1->Matrix.M[1][1] +
          Multiplicand0->Matrix.M[0][2] * Multiplicand1->Matrix.M[2][1] +
          Multiplicand0->Matrix.M[0][3] * Multiplicand1->Matrix.M[3][1];

    M02 = Multiplicand0->Matrix.M[0][0] * Multiplicand1->Matrix.M[0][2] + 
          Multiplicand0->Matrix.M[0][1] * Multiplicand1->Matrix.M[1][2] +
          Multiplicand0->Matrix.M[0][2] * Multiplicand1->Matrix.M[2][2] +
          Multiplicand0->Matrix.M[0][3] * Multiplicand1->Matrix.M[3][2];

    M03 = Multiplicand0->Matrix.M[0][0] * Multiplicand1->Matrix.M[0][3] + 
          Multiplicand0->Matrix.M[0][1] * Multiplicand1->Matrix.M[1][3] +
          Multiplicand0->Matrix.M[0][2] * Multiplicand1->Matrix.M[2][3] +
          Multiplicand0->Matrix.M[0][3] * Multiplicand1->Matrix.M[3][3];

    M10 = Multiplicand0->Matrix.M[1][0] * Multiplicand1->Matrix.M[0][0] + 
          Multiplicand0->Matrix.M[1][1] * Multiplicand1->Matrix.M[1][0] +
          Multiplicand0->Matrix.M[1][2] * Multiplicand1->Matrix.M[2][0] +
          Multiplicand0->Matrix.M[1][3] * Multiplicand1->Matrix.M[3][0];

    M11 = Multiplicand0->Matrix.M[1][0] * Multiplicand1->Matrix.M[0][1] + 
          Multiplicand0->Matrix.M[1][1] * Multiplicand1->Matrix.M[1][1] +
          Multiplicand0->Matrix.M[1][2] * Multiplicand1->Matrix.M[2][1] +
          Multiplicand0->Matrix.M[1][3] * Multiplicand1->Matrix.M[3][1];

    M12 = Multiplicand0->Matrix.M[1][0] * Multiplicand1->Matrix.M[0][2] + 
          Multiplicand0->Matrix.M[1][1] * Multiplicand1->Matrix.M[1][2] +
          Multiplicand0->Matrix.M[1][2] * Multiplicand1->Matrix.M[2][2] +
          Multiplicand0->Matrix.M[1][3] * Multiplicand1->Matrix.M[3][2];

    M13 = Multiplicand0->Matrix.M[1][0] * Multiplicand1->Matrix.M[0][3] + 
          Multiplicand0->Matrix.M[1][1] * Multiplicand1->Matrix.M[1][3] +
          Multiplicand0->Matrix.M[1][2] * Multiplicand1->Matrix.M[2][3] +
          Multiplicand0->Matrix.M[1][3] * Multiplicand1->Matrix.M[3][3];

    M20 = Multiplicand0->Matrix.M[2][0] * Multiplicand1->Matrix.M[0][0] + 
          Multiplicand0->Matrix.M[2][1] * Multiplicand1->Matrix.M[1][0] +
          Multiplicand0->Matrix.M[2][2] * Multiplicand1->Matrix.M[2][0] +
          Multiplicand0->Matrix.M[2][3] * Multiplicand1->Matrix.M[3][0];

    M21 = Multiplicand0->Matrix.M[2][0] * Multiplicand1->Matrix.M[0][1] + 
          Multiplicand0->Matrix.M[2][1] * Multiplicand1->Matrix.M[1][1] +
          Multiplicand0->Matrix.M[2][2] * Multiplicand1->Matrix.M[2][1] +
          Multiplicand0->Matrix.M[2][3] * Multiplicand1->Matrix.M[3][1];

    M22 = Multiplicand0->Matrix.M[2][0] * Multiplicand1->Matrix.M[0][2] + 
          Multiplicand0->Matrix.M[2][1] * Multiplicand1->Matrix.M[1][2] +
          Multiplicand0->Matrix.M[2][2] * Multiplicand1->Matrix.M[2][2] +
          Multiplicand0->Matrix.M[2][3] * Multiplicand1->Matrix.M[3][2];

    M23 = Multiplicand0->Matrix.M[2][0] * Multiplicand1->Matrix.M[0][3] + 
          Multiplicand0->Matrix.M[2][1] * Multiplicand1->Matrix.M[1][3] +
          Multiplicand0->Matrix.M[2][2] * Multiplicand1->Matrix.M[2][3] +
          Multiplicand0->Matrix.M[2][3] * Multiplicand1->Matrix.M[3][3];

    M30 = Multiplicand0->Matrix.M[3][0] * Multiplicand1->Matrix.M[0][0] + 
          Multiplicand0->Matrix.M[3][1] * Multiplicand1->Matrix.M[1][0] +
          Multiplicand0->Matrix.M[3][2] * Multiplicand1->Matrix.M[2][0] +
          Multiplicand0->Matrix.M[3][3] * Multiplicand1->Matrix.M[3][0];

    M31 = Multiplicand0->Matrix.M[3][0] * Multiplicand1->Matrix.M[0][1] + 
          Multiplicand0->Matrix.M[3][1] * Multiplicand1->Matrix.M[1][1] +
          Multiplicand0->Matrix.M[3][2] * Multiplicand1->Matrix.M[2][1] +
          Multiplicand0->Matrix.M[3][3] * Multiplicand1->Matrix.M[3][1];

    M32 = Multiplicand0->Matrix.M[3][0] * Multiplicand1->Matrix.M[0][2] + 
          Multiplicand0->Matrix.M[3][1] * Multiplicand1->Matrix.M[1][2] +
          Multiplicand0->Matrix.M[3][2] * Multiplicand1->Matrix.M[2][2] +
          Multiplicand0->Matrix.M[3][3] * Multiplicand1->Matrix.M[3][2];

    M33 = Multiplicand0->Matrix.M[3][0] * Multiplicand1->Matrix.M[0][3] + 
          Multiplicand0->Matrix.M[3][1] * Multiplicand1->Matrix.M[1][3] +
          Multiplicand0->Matrix.M[3][2] * Multiplicand1->Matrix.M[2][3] +
          Multiplicand0->Matrix.M[3][3] * Multiplicand1->Matrix.M[3][3];

    Status = MatrixInitialize(Product,
                              M00,
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
                              M33);

    return Status;
}