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
    ASSERT(!IsNaNFloat(Divisor));
    ASSERT(!IsInfFloat(Divisor));
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
    ASSERT(!IsNaNFloat(Scalar));
    ASSERT(!IsInfFloat(Scalar));

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
    ASSERT(!IsNaNFloat(M00));
    ASSERT(!IsInfFloat(M00));
    ASSERT(!IsNaNFloat(M01));
    ASSERT(!IsInfFloat(M01));
    ASSERT(!IsNaNFloat(M02));
    ASSERT(!IsInfFloat(M02));
    ASSERT(!IsNaNFloat(M03));
    ASSERT(!IsInfFloat(M03));
    ASSERT(!IsNaNFloat(M10));
    ASSERT(!IsInfFloat(M10));
    ASSERT(!IsNaNFloat(M11));
    ASSERT(!IsInfFloat(M11));
    ASSERT(!IsNaNFloat(M12));
    ASSERT(!IsInfFloat(M12));
    ASSERT(!IsNaNFloat(M13));
    ASSERT(!IsInfFloat(M13));
    ASSERT(!IsNaNFloat(M20));
    ASSERT(!IsInfFloat(M20));
    ASSERT(!IsNaNFloat(M21));
    ASSERT(!IsInfFloat(M21));
    ASSERT(!IsNaNFloat(M22));
    ASSERT(!IsInfFloat(M22));
    ASSERT(!IsNaNFloat(M23));
    ASSERT(!IsInfFloat(M23));
    ASSERT(!IsNaNFloat(M30));
    ASSERT(!IsInfFloat(M30));
    ASSERT(!IsNaNFloat(M31));
    ASSERT(!IsInfFloat(M31));
    ASSERT(!IsNaNFloat(M32));
    ASSERT(!IsInfFloat(M32));
    ASSERT(!IsNaNFloat(M33));
    ASSERT(!IsInfFloat(M33));
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
    ASSERT(!IsNaNFloat(M00));
    ASSERT(!IsInfFloat(M00));
    ASSERT(!IsNaNFloat(M01));
    ASSERT(!IsInfFloat(M01));
    ASSERT(!IsNaNFloat(M02));
    ASSERT(!IsInfFloat(M02));
    ASSERT(!IsNaNFloat(M03));
    ASSERT(!IsInfFloat(M03));
    ASSERT(!IsNaNFloat(M10));
    ASSERT(!IsInfFloat(M10));
    ASSERT(!IsNaNFloat(M11));
    ASSERT(!IsInfFloat(M11));
    ASSERT(!IsNaNFloat(M12));
    ASSERT(!IsInfFloat(M12));
    ASSERT(!IsNaNFloat(M13));
    ASSERT(!IsInfFloat(M13));
    ASSERT(!IsNaNFloat(M20));
    ASSERT(!IsInfFloat(M20));
    ASSERT(!IsNaNFloat(M21));
    ASSERT(!IsInfFloat(M21));
    ASSERT(!IsNaNFloat(M22));
    ASSERT(!IsInfFloat(M22));
    ASSERT(!IsNaNFloat(M23));
    ASSERT(!IsInfFloat(M23));
    ASSERT(!IsNaNFloat(M30));
    ASSERT(!IsInfFloat(M30));
    ASSERT(!IsNaNFloat(M31));
    ASSERT(!IsInfFloat(M31));
    ASSERT(!IsNaNFloat(M32));
    ASSERT(!IsInfFloat(M32));
    ASSERT(!IsNaNFloat(M33));
    ASSERT(!IsInfFloat(M33));

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

VOID
MatrixInitializeIdentity(
    __out PINVERTIBLE_MATRIX Matrix
    )
{
    ASSERT(Matrix != NULL);

    MatrixpInitializeIdentity(&Matrix->Matrix);
    MatrixpInitializeIdentity(&Matrix->Inverse);
}

VOID
MatrixInitializeTranslation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    )
{
    ASSERT(Matrix != NULL);
    ASSERT(!IsNaNFloat(X));
    ASSERT(!IsInfFloat(X));
    ASSERT(!IsNaNFloat(Y));
    ASSERT(!IsInfFloat(Y));
    ASSERT(!IsNaNFloat(Z));
    ASSERT(!IsInfFloat(Z));

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
}

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeScalar(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixInitializeRotation(
    __out PINVERTIBLE_MATRIX Matrix,
    __in FLOAT Theta,
    __in FLOAT X,
    __in FLOAT Y,
    __in FLOAT Z
    );

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
    );

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
    );

__success(return == ISTATUS_SUCCESS)
ISTATUS
MatrixMultiply(
    __in PINVERTIBLE_MATRIX Multiplicand0,
    __in PINVERTIBLE_MATRIX Multiplicand1,
    __out PINVERTIBLE_MATRIX Product
    );