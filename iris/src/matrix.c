/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    matrix.c

Abstract:

    This module implements the Iris matrix functions.

--*/

#include <irisp.h>

//
// Static Functions
//

STATIC
VOID
MatrixpDivideRow(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T Row,
    _In_ FLOAT Divisor
    )
{
    FLOAT Scalar;

    ASSERT(Matrix != NULL);
    ASSERT(0 <= Row);
    ASSERT(Row <= 3);
    ASSERT(IsNormalFloat(Divisor) != FALSE);
    ASSERT(IsFiniteFloat(Divisor) != FALSE);
    ASSERT(IsZeroFloat(Divisor) == FALSE);

    Scalar = (FLOAT) 1.0 / Divisor;

    Matrix[Row][0] *= Divisor;
    Matrix[Row][1] *= Divisor;
    Matrix[Row][2] *= Divisor;
    Matrix[Row][3] *= Divisor;
}

STATIC
VOID
MatrixpScaledSubtractRow(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T ConstantRow,
    _In_range_(0, 3) SIZE_T ModifiedRow,
    _In_ FLOAT Scalar
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(0 <= ConstantRow);
    ASSERT(ConstantRow <= 3);
    ASSERT(0 <= ModifiedRow);
    ASSERT(ModifiedRow <= 3);
    ASSERT(IsNormalFloat(Scalar) != FALSE);
    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    Matrix[ModifiedRow][0] -= Matrix[ConstantRow][0] * Scalar;
    Matrix[ModifiedRow][1] -= Matrix[ConstantRow][1] * Scalar;
    Matrix[ModifiedRow][2] -= Matrix[ConstantRow][2] * Scalar;
    Matrix[ModifiedRow][3] -= Matrix[ConstantRow][3] * Scalar;
}

STATIC
VOID
MatrixpSwapRows(
    _Inout_ FLOAT Matrix[4][4],
    _In_range_(0, 3) SIZE_T Row0,
    _In_range_(0, 3) SIZE_T Row1
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
        Temp = Matrix[Row0][Index];
        Matrix[Row0][Index] = Matrix[Row1][Index];
        Matrix[Row1][Index] = Temp;
    }
}

STATIC
VOID
MatrixpInitialize(
    _Out_ PMATRIX Matrix,
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
    _In_ PMATRIX Inverse,
    _In_ PINVERTIBLE_MATRIX InvertibleMatrix
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(IsNormalFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsNormalFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsNormalFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsNormalFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsNormalFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsNormalFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsNormalFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsNormalFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsNormalFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsNormalFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsNormalFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsNormalFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsNormalFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsNormalFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsNormalFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsNormalFloat(M33) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);
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
    Matrix->InvertibleMatrix = InvertibleMatrix;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
MatrixpInvert(
    _Out_writes_(4) FLOAT Inverse[4][4],
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
    _In_ FLOAT M33
    )
{
    FLOAT Matrix[4][4];
    SIZE_T ColumnIndex;
    SIZE_T CurrentRow;
    SIZE_T RowIndex;

    ASSERT(Inverse != NULL);
    ASSERT(IsNormalFloat(M00) != FALSE);
    ASSERT(IsFiniteFloat(M00) != FALSE);
    ASSERT(IsNormalFloat(M01) != FALSE);
    ASSERT(IsFiniteFloat(M01) != FALSE);
    ASSERT(IsNormalFloat(M02) != FALSE);
    ASSERT(IsFiniteFloat(M02) != FALSE);
    ASSERT(IsNormalFloat(M03) != FALSE);
    ASSERT(IsFiniteFloat(M03) != FALSE);
    ASSERT(IsNormalFloat(M10) != FALSE);
    ASSERT(IsFiniteFloat(M10) != FALSE);
    ASSERT(IsNormalFloat(M11) != FALSE);
    ASSERT(IsFiniteFloat(M11) != FALSE);
    ASSERT(IsNormalFloat(M12) != FALSE);
    ASSERT(IsFiniteFloat(M12) != FALSE);
    ASSERT(IsNormalFloat(M13) != FALSE);
    ASSERT(IsFiniteFloat(M13) != FALSE);
    ASSERT(IsNormalFloat(M20) != FALSE);
    ASSERT(IsFiniteFloat(M20) != FALSE);
    ASSERT(IsNormalFloat(M21) != FALSE);
    ASSERT(IsFiniteFloat(M21) != FALSE);
    ASSERT(IsNormalFloat(M22) != FALSE);
    ASSERT(IsFiniteFloat(M22) != FALSE);
    ASSERT(IsNormalFloat(M23) != FALSE);
    ASSERT(IsFiniteFloat(M23) != FALSE);
    ASSERT(IsNormalFloat(M30) != FALSE);
    ASSERT(IsFiniteFloat(M30) != FALSE);
    ASSERT(IsNormalFloat(M31) != FALSE);
    ASSERT(IsFiniteFloat(M31) != FALSE);
    ASSERT(IsNormalFloat(M32) != FALSE);
    ASSERT(IsFiniteFloat(M32) != FALSE);
    ASSERT(IsNormalFloat(M33) != FALSE);
    ASSERT(IsFiniteFloat(M33) != FALSE);

    Inverse[0][0] = (FLOAT) 1.0;
    Inverse[0][1] = (FLOAT) 0.0;
    Inverse[0][2] = (FLOAT) 0.0;
    Inverse[0][3] = (FLOAT) 0.0;
    Inverse[1][0] = (FLOAT) 0.0;
    Inverse[1][1] = (FLOAT) 1.0;
    Inverse[1][2] = (FLOAT) 0.0;
    Inverse[1][3] = (FLOAT) 0.0;
    Inverse[2][0] = (FLOAT) 0.0;
    Inverse[2][1] = (FLOAT) 0.0;
    Inverse[2][2] = (FLOAT) 1.0;
    Inverse[2][3] = (FLOAT) 0.0;
    Inverse[3][0] = (FLOAT) 0.0;
    Inverse[3][1] = (FLOAT) 0.0;
    Inverse[3][2] = (FLOAT) 0.0;
    Inverse[3][3] = (FLOAT) 1.0;

    Matrix[0][0] = M00;
    Matrix[0][1] = M01;
    Matrix[0][2] = M02;
    Matrix[0][3] = M03;
    Matrix[1][0] = M10;
    Matrix[1][1] = M11;
    Matrix[1][2] = M12;
    Matrix[1][3] = M13;
    Matrix[2][0] = M20;
    Matrix[2][1] = M21;
    Matrix[2][2] = M22;
    Matrix[2][3] = M23;
    Matrix[3][0] = M30;
    Matrix[3][1] = M31;
    Matrix[3][2] = M32;
    Matrix[3][3] = M33;

    for (CurrentRow = 0, ColumnIndex = 0; 
         CurrentRow < 4 && ColumnIndex < 4; 
         CurrentRow++, ColumnIndex++)
    {
        for (ColumnIndex = ColumnIndex;
             ColumnIndex < 4;
             ColumnIndex++)
        {
            for (RowIndex = CurrentRow;
                 RowIndex < 4;
                 RowIndex++)
            {
                if (IsZeroFloat(Matrix[RowIndex][ColumnIndex]) == FALSE)
                {
                    break;
                }
            }

            if (IsZeroFloat(Matrix[RowIndex][ColumnIndex]) == FALSE)
            {
                break;
            }
        }

        if (ColumnIndex == 4)
        {
            return ISTATUS_ARITHMETIC_ERROR;
        }

        if (RowIndex != CurrentRow)
        {
            MatrixpSwapRows(Matrix, RowIndex, CurrentRow);
            MatrixpSwapRows(Inverse, RowIndex, CurrentRow);
        }

        MatrixpDivideRow(Inverse, CurrentRow, Matrix[CurrentRow][ColumnIndex]);
        MatrixpDivideRow(Matrix, CurrentRow, Matrix[CurrentRow][ColumnIndex]);

        for (RowIndex = 0; RowIndex < 4; RowIndex++)
        {
            if (RowIndex == CurrentRow)
            {
                continue;
            }

            MatrixpScaledSubtractRow(Inverse, 
                                     CurrentRow, 
                                     RowIndex,
                                     Matrix[RowIndex][ColumnIndex]);

            MatrixpScaledSubtractRow(Matrix,
                                     CurrentRow,
                                     RowIndex,
                                     Matrix[RowIndex][ColumnIndex]);
        }
    }

    return ISTATUS_SUCCESS;
}

//
// Public Functions
//

_Check_return_
_Ret_opt_
PMATRIX
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
    _In_ FLOAT M33
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;
    FLOAT Inverse[4][4];
    ISTATUS Status;

    if (!IsNormalFloat(M00) != FALSE || !IsFiniteFloat(M00) != FALSE ||
        !IsNormalFloat(M01) != FALSE || !IsFiniteFloat(M01) != FALSE ||
        !IsNormalFloat(M02) != FALSE || !IsFiniteFloat(M02) != FALSE ||
        !IsNormalFloat(M03) != FALSE || !IsFiniteFloat(M03) != FALSE ||
        !IsNormalFloat(M10) != FALSE || !IsFiniteFloat(M10) != FALSE ||
        !IsNormalFloat(M11) != FALSE || !IsFiniteFloat(M11) != FALSE ||
        !IsNormalFloat(M12) != FALSE || !IsFiniteFloat(M12) != FALSE ||
        !IsNormalFloat(M13) != FALSE || !IsFiniteFloat(M13) != FALSE ||
        !IsNormalFloat(M20) != FALSE || !IsFiniteFloat(M20) != FALSE ||
        !IsNormalFloat(M21) != FALSE || !IsFiniteFloat(M21) != FALSE ||
        !IsNormalFloat(M22) != FALSE || !IsFiniteFloat(M22) != FALSE ||
        !IsNormalFloat(M23) != FALSE || !IsFiniteFloat(M23) != FALSE ||
        !IsNormalFloat(M30) != FALSE || !IsFiniteFloat(M30) != FALSE ||
        !IsNormalFloat(M31) != FALSE || !IsFiniteFloat(M31) != FALSE ||
        !IsNormalFloat(M32) != FALSE || !IsFiniteFloat(M32) != FALSE ||
        !IsNormalFloat(M33) != FALSE || !IsFiniteFloat(M33) != FALSE)
    {
        return NULL;
    }

    Status = MatrixpInvert(Inverse,
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

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    InvertibleMatrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (InvertibleMatrix == NULL)
    {
        return NULL;
    }

    MatrixpInitialize(&InvertibleMatrix->Matrix,
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
                      &InvertibleMatrix->Inverse,
                      InvertibleMatrix);

    MatrixpInitialize(&InvertibleMatrix->Inverse,
                      Inverse[0][0],
                      Inverse[0][1],
                      Inverse[0][2],
                      Inverse[0][3],
                      Inverse[1][0],
                      Inverse[1][1],
                      Inverse[1][2],
                      Inverse[1][3],
                      Inverse[2][0],
                      Inverse[2][1],
                      Inverse[2][2],
                      Inverse[2][3],
                      Inverse[3][0],
                      Inverse[3][1],
                      Inverse[3][2],
                      Inverse[3][3],
                      &InvertibleMatrix->Matrix,
                      InvertibleMatrix);

    InvertibleMatrix->ReferenceCount = 1;

    return &InvertibleMatrix->Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateTranslation(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;

    if (!IsNormalFloat(X) != FALSE || !IsFiniteFloat(X) != FALSE ||
        !IsNormalFloat(Y) != FALSE || !IsFiniteFloat(Y) != FALSE ||
        !IsNormalFloat(Z) != FALSE || !IsFiniteFloat(Z) != FALSE)
    {
        return NULL;
    }

    InvertibleMatrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (InvertibleMatrix == NULL)
    {
        return NULL;
    }

    MatrixpInitialize(&InvertibleMatrix->Matrix, 
                      (FLOAT) 1.0,
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
                      &InvertibleMatrix->Inverse,
                      InvertibleMatrix);

    MatrixpInitialize(&InvertibleMatrix->Inverse, 
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
                      &InvertibleMatrix->Matrix,
                      InvertibleMatrix);

    InvertibleMatrix->ReferenceCount = 1;

    return &InvertibleMatrix->Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateScalar(
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    PINVERTIBLE_MATRIX InvertibleMatrix;

    if (!IsNormalFloat(X) != FALSE || !IsFiniteFloat(X) != FALSE ||
        !IsNormalFloat(Y) != FALSE || !IsFiniteFloat(Y) != FALSE ||
        !IsNormalFloat(Z) != FALSE || !IsFiniteFloat(Z) != FALSE)
    {
        return NULL;
    }

    InvertibleMatrix = malloc(sizeof(INVERTIBLE_MATRIX));

    if (InvertibleMatrix == NULL)
    {
        return NULL;
    }

    MatrixpInitialize(&InvertibleMatrix->Matrix, 
                      X,
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
                      &InvertibleMatrix->Inverse,
                      InvertibleMatrix);

    MatrixpInitialize(&InvertibleMatrix->Inverse, 
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
                      &InvertibleMatrix->Inverse,
                      InvertibleMatrix);

    InvertibleMatrix->ReferenceCount = 1;

    return &InvertibleMatrix->Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixpAllocateRotation(
    _In_ FLOAT Theta,
    _In_ VECTOR3 Axis
    )
{
    VECTOR3 NormalizedAxis;
    PMATRIX Matrix;
    FLOAT Length;
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

    ASSERT(IsNormalFloat(Theta) != FALSE);
    ASSERT(IsFiniteFloat(Theta) != FALSE);
    ASSERT(IsNormalFloat(Axis.X) != FALSE);
    ASSERT(IsFiniteFloat(Axis.X) != FALSE);
    ASSERT(IsNormalFloat(Axis.Y) != FALSE);
    ASSERT(IsFiniteFloat(Axis.Y) != FALSE);
    ASSERT(IsNormalFloat(Axis.Z) != FALSE);
    ASSERT(IsFiniteFloat(Axis.Z) != FALSE);

    Length = VectorLength(Axis);

    if (IsZeroFloat(Length) != FALSE)
    {
        return NULL;
    }

    NormalizedAxis = VectorNormalize(Axis, NULL);

    Sin = SinFloat(Theta * IRIS_PI / (FLOAT) 180.0);
    Cos = CosFloat(Theta * IRIS_PI / (FLOAT) 180.0);
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

    Matrix = MatrixAllocate(M00, 
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
                            (FLOAT) 1.0);

    return Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateRotation(
    _In_ FLOAT Theta,
    _In_ FLOAT X,
    _In_ FLOAT Y,
    _In_ FLOAT Z
    )
{
    PMATRIX Matrix;
    VECTOR3 Axis;

    if (!IsNormalFloat(Theta) != FALSE || !IsFiniteFloat(Theta) != FALSE ||
        !IsNormalFloat(X) != FALSE || !IsFiniteFloat(X) != FALSE ||
        !IsNormalFloat(Y) != FALSE || !IsFiniteFloat(Y) != FALSE ||
        !IsNormalFloat(Z) != FALSE || !IsFiniteFloat(Z) != FALSE)
    {
        return NULL;
    }

    Axis = VectorCreate(X, Y, Z);

    Matrix = MatrixpAllocateRotation(Theta, Axis);

    return Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateOrthographic(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    )
{
    PMATRIX Matrix;
    FLOAT Tx;
    FLOAT Ty;
    FLOAT Tz;
    FLOAT Sx;
    FLOAT Sy;
    FLOAT Sz;

    if (!IsNormalFloat(Left) != FALSE || !IsFiniteFloat(Left) != FALSE ||
        !IsNormalFloat(Right) != FALSE || !IsFiniteFloat(Right) != FALSE ||
        !IsNormalFloat(Bottom) != FALSE || !IsFiniteFloat(Bottom) != FALSE ||
        !IsNormalFloat(Top) != FALSE || !IsFiniteFloat(Top) != FALSE ||
        !IsNormalFloat(Near) != FALSE || !IsFiniteFloat(Near) != FALSE ||
        !IsNormalFloat(Far) != FALSE || !IsFiniteFloat(Far) != FALSE)
    {
        return NULL;
    }

    if (Left == Right || Bottom == Top || Near == Far)
    {
        return NULL;
    }

    Tx = -(Right + Left) / (Right - Left);
    Ty = -(Top + Bottom) / (Top - Bottom);
    Tz = -(Far + Near) / (Far - Near);

    Sx = (FLOAT) 2.0 / (Right - Left);
    Sy = (FLOAT) 2.0 / (Top - Bottom);
    Sz = (FLOAT) -2.0 / (Far - Near);

    Matrix = MatrixAllocate(Sx,
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
                            (FLOAT) 1.0);

    return Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateFrustum(
    _In_ FLOAT Left,
    _In_ FLOAT Right,
    _In_ FLOAT Bottom,
    _In_ FLOAT Top,
    _In_ FLOAT Near,
    _In_ FLOAT Far
    )
{
    PMATRIX Matrix;
    FLOAT Sx;
    FLOAT Sy;
    FLOAT A;
    FLOAT B;
    FLOAT C;
    FLOAT D;

    if (!IsNormalFloat(Left) != FALSE || !IsFiniteFloat(Left) != FALSE ||
        !IsNormalFloat(Right) != FALSE || !IsFiniteFloat(Right) != FALSE ||
        !IsNormalFloat(Bottom) != FALSE || !IsFiniteFloat(Bottom) != FALSE ||
        !IsNormalFloat(Top) != FALSE || !IsFiniteFloat(Top) != FALSE ||
        !IsNormalFloat(Near) != FALSE || !IsFiniteFloat(Near) != FALSE ||
        !IsNormalFloat(Far) != FALSE || !IsFiniteFloat(Far) != FALSE)
    {
        return NULL;
    }

    if (Left == Right || Bottom == Top || Near == Far || 
        Near <= (FLOAT) 0.0 || Far <= (FLOAT) 0.0)
    {
        return NULL;
    }

    Sx = ((FLOAT) 2.0 * Near) / (Right - Left);
    Sy = ((FLOAT) 2.0 * Near) / (Top - Bottom);

    A = (Right + Left) / (Right - Left);
    B = (Top + Bottom) / (Top - Bottom);

    C = -(Far + Near) / (Far - Near);
    D = (FLOAT) -2.0 * Far * Near / (Far - Near);

    Matrix = MatrixAllocate(Sx,
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
                            (FLOAT) 0.0);

    return Matrix;
}

_Check_return_
_When_(Multiplicand0 == NULL && Multiplicand1 == NULL, _Ret_null_)
_When_(Multiplicand0 != NULL || Multiplicand1 != NULL, _Ret_opt_)
PMATRIX
MatrixAllocateProduct(
    _In_opt_ PMATRIX Multiplicand0,
    _In_opt_ PMATRIX Multiplicand1
    )
{
    PMATRIX Matrix;
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

    if (Multiplicand0 == NULL && Multiplicand1 == NULL)
    {
        return NULL;
    }

    if (Multiplicand0 == NULL)
    {
        MatrixReference(Multiplicand1);
        return Multiplicand1;
    }

    if (Multiplicand1 == NULL)
    {
        MatrixReference(Multiplicand0);
        return Multiplicand0;
    }

    M00 = Multiplicand0->M[0][0] * Multiplicand1->M[0][0] + 
          Multiplicand0->M[0][1] * Multiplicand1->M[1][0] +
          Multiplicand0->M[0][2] * Multiplicand1->M[2][0] +
          Multiplicand0->M[0][3] * Multiplicand1->M[3][0];

    M01 = Multiplicand0->M[0][0] * Multiplicand1->M[0][1] + 
          Multiplicand0->M[0][1] * Multiplicand1->M[1][1] +
          Multiplicand0->M[0][2] * Multiplicand1->M[2][1] +
          Multiplicand0->M[0][3] * Multiplicand1->M[3][1];

    M02 = Multiplicand0->M[0][0] * Multiplicand1->M[0][2] + 
          Multiplicand0->M[0][1] * Multiplicand1->M[1][2] +
          Multiplicand0->M[0][2] * Multiplicand1->M[2][2] +
          Multiplicand0->M[0][3] * Multiplicand1->M[3][2];

    M03 = Multiplicand0->M[0][0] * Multiplicand1->M[0][3] + 
          Multiplicand0->M[0][1] * Multiplicand1->M[1][3] +
          Multiplicand0->M[0][2] * Multiplicand1->M[2][3] +
          Multiplicand0->M[0][3] * Multiplicand1->M[3][3];

    M10 = Multiplicand0->M[1][0] * Multiplicand1->M[0][0] + 
          Multiplicand0->M[1][1] * Multiplicand1->M[1][0] +
          Multiplicand0->M[1][2] * Multiplicand1->M[2][0] +
          Multiplicand0->M[1][3] * Multiplicand1->M[3][0];

    M11 = Multiplicand0->M[1][0] * Multiplicand1->M[0][1] + 
          Multiplicand0->M[1][1] * Multiplicand1->M[1][1] +
          Multiplicand0->M[1][2] * Multiplicand1->M[2][1] +
          Multiplicand0->M[1][3] * Multiplicand1->M[3][1];

    M12 = Multiplicand0->M[1][0] * Multiplicand1->M[0][2] + 
          Multiplicand0->M[1][1] * Multiplicand1->M[1][2] +
          Multiplicand0->M[1][2] * Multiplicand1->M[2][2] +
          Multiplicand0->M[1][3] * Multiplicand1->M[3][2];

    M13 = Multiplicand0->M[1][0] * Multiplicand1->M[0][3] + 
          Multiplicand0->M[1][1] * Multiplicand1->M[1][3] +
          Multiplicand0->M[1][2] * Multiplicand1->M[2][3] +
          Multiplicand0->M[1][3] * Multiplicand1->M[3][3];

    M20 = Multiplicand0->M[2][0] * Multiplicand1->M[0][0] + 
          Multiplicand0->M[2][1] * Multiplicand1->M[1][0] +
          Multiplicand0->M[2][2] * Multiplicand1->M[2][0] +
          Multiplicand0->M[2][3] * Multiplicand1->M[3][0];

    M21 = Multiplicand0->M[2][0] * Multiplicand1->M[0][1] + 
          Multiplicand0->M[2][1] * Multiplicand1->M[1][1] +
          Multiplicand0->M[2][2] * Multiplicand1->M[2][1] +
          Multiplicand0->M[2][3] * Multiplicand1->M[3][1];

    M22 = Multiplicand0->M[2][0] * Multiplicand1->M[0][2] + 
          Multiplicand0->M[2][1] * Multiplicand1->M[1][2] +
          Multiplicand0->M[2][2] * Multiplicand1->M[2][2] +
          Multiplicand0->M[2][3] * Multiplicand1->M[3][2];

    M23 = Multiplicand0->M[2][0] * Multiplicand1->M[0][3] + 
          Multiplicand0->M[2][1] * Multiplicand1->M[1][3] +
          Multiplicand0->M[2][2] * Multiplicand1->M[2][3] +
          Multiplicand0->M[2][3] * Multiplicand1->M[3][3];

    M30 = Multiplicand0->M[3][0] * Multiplicand1->M[0][0] + 
          Multiplicand0->M[3][1] * Multiplicand1->M[1][0] +
          Multiplicand0->M[3][2] * Multiplicand1->M[2][0] +
          Multiplicand0->M[3][3] * Multiplicand1->M[3][0];

    M31 = Multiplicand0->M[3][0] * Multiplicand1->M[0][1] + 
          Multiplicand0->M[3][1] * Multiplicand1->M[1][1] +
          Multiplicand0->M[3][2] * Multiplicand1->M[2][1] +
          Multiplicand0->M[3][3] * Multiplicand1->M[3][1];

    M32 = Multiplicand0->M[3][0] * Multiplicand1->M[0][2] + 
          Multiplicand0->M[3][1] * Multiplicand1->M[1][2] +
          Multiplicand0->M[3][2] * Multiplicand1->M[2][2] +
          Multiplicand0->M[3][3] * Multiplicand1->M[3][2];

    M33 = Multiplicand0->M[3][0] * Multiplicand1->M[0][3] + 
          Multiplicand0->M[3][1] * Multiplicand1->M[1][3] +
          Multiplicand0->M[3][2] * Multiplicand1->M[2][3] +
          Multiplicand0->M[3][3] * Multiplicand1->M[3][3];

    Matrix = MatrixAllocate(M00,
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

    return Matrix;
}

_Check_return_
_Ret_opt_
PMATRIX
MatrixAllocateInverse(
    _In_opt_ PMATRIX Matrix
    )
{
    PMATRIX Inverse;

    if (Matrix == NULL)
    {
        return NULL;
    }

    if (Matrix->InvertibleMatrix != NULL)
    {
        Matrix->InvertibleMatrix->ReferenceCount += 1;
    }
    
    Inverse = Matrix->Inverse;

    return Inverse;
}

ISTATUS
MatrixReadContents(
    _In_ PCMATRIX Matrix,
    _Out_writes_(4) FLOAT Contents[4][4]
    )
{
    if (Matrix == NULL || Contents == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Contents[0][0] = Matrix->M[0][0];
    Contents[0][1] = Matrix->M[0][1];
    Contents[0][2] = Matrix->M[0][2];
    Contents[0][3] = Matrix->M[0][3];
    Contents[1][0] = Matrix->M[1][0];
    Contents[1][1] = Matrix->M[1][1];
    Contents[1][2] = Matrix->M[1][2];
    Contents[1][3] = Matrix->M[1][3];
    Contents[2][0] = Matrix->M[2][0];
    Contents[2][1] = Matrix->M[2][1];
    Contents[2][2] = Matrix->M[2][2];
    Contents[2][3] = Matrix->M[2][3];
    Contents[3][0] = Matrix->M[3][0];
    Contents[3][1] = Matrix->M[3][1];
    Contents[3][2] = Matrix->M[3][2];
    Contents[3][3] = Matrix->M[3][3];

    return ISTATUS_SUCCESS;
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