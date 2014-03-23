/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    matrix.c

Abstract:

    This file contains tests for the INVERTIBLE_MATRIX type.

--*/

#include <iristest.h>

TEST(MatrixInitialize)
{
    INVERTIBLE_MATRIX Matrix;
    ISTATUS Status;

    Status = MatrixInitialize(&Matrix,
                              (FLOAT) 1.0,
                              (FLOAT) 2.0,
                              (FLOAT) 3.0,
                              (FLOAT) 4.0,
                              (FLOAT) 5.0,
                              (FLOAT) 6.0,
                              (FLOAT) 7.0,
                              (FLOAT) 8.0,
                              (FLOAT) 9.0,
                              (FLOAT) 10.0,
                              (FLOAT) 11.0,
                              (FLOAT) 12.0,
                              (FLOAT) 13.0,
                              (FLOAT) 14.0,
                              (FLOAT) 15.0,
                              (FLOAT) 16.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 3.0, Matrix.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 4.0, Matrix.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 5.0, Matrix.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 6.0, Matrix.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 7.0, Matrix.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 8.0, Matrix.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 9.0, Matrix.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 10.0, Matrix.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 11.0, Matrix.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 12.0, Matrix.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 13.0, Matrix.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 14.0, Matrix.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 15.0, Matrix.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 16.0, Matrix.Matrix.M[3][3] );

    CHECK_EQUAL(&Matrix.Inverse, Matrix.Matrix.Inverse);
    CHECK_EQUAL(&Matrix.Matrix, Matrix.Inverse.Inverse);
}

TEST(MatrixInverse)
{
    INVERTIBLE_MATRIX Matrix;
    ISTATUS Status;

    Status = MatrixInitialize(&Matrix,
                              (FLOAT) 1.0,
                              (FLOAT) 0.0,
                              (FLOAT) 0.0,
                              (FLOAT) 1.0,
                              (FLOAT) 0.0,
                              (FLOAT) 1.0,
                              (FLOAT) 0.0,
                              (FLOAT) 1.0,
                              (FLOAT) 0.0,
                              (FLOAT) 0.0,
                              (FLOAT) 1.0,
                              (FLOAT) 1.0,
                              (FLOAT) 0.0,
                              (FLOAT) 0.0,
                              (FLOAT) 0.0,
                              (FLOAT) 1.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[3][3] );

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Inverse.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[0][2] );
    CHECK_EQUAL((FLOAT) -1.0, Matrix.Inverse.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[1][0] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Inverse.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[1][2] );
    CHECK_EQUAL((FLOAT) -1.0, Matrix.Inverse.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[2][1] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Inverse.M[2][2] );
    CHECK_EQUAL((FLOAT) -1.0, Matrix.Inverse.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Inverse.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Inverse.M[3][3] );

    CHECK_EQUAL(&Matrix.Inverse, Matrix.Matrix.Inverse);
    CHECK_EQUAL(&Matrix.Matrix, Matrix.Inverse.Inverse);
}

TEST(MatrixInitializeIdentity)
{
    INVERTIBLE_MATRIX Matrix;
    ISTATUS Status;

    Status = MatrixInitializeIdentity(&Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[3][3] );
}

TEST(MatrixInitializeTranslation)
{
    INVERTIBLE_MATRIX Matrix;
    ISTATUS Status;

    Status = MatrixInitializeTranslation(&Matrix,
                                         (FLOAT) 1.0,
                                         (FLOAT) 2.0,
                                         (FLOAT) 3.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 3.0, Matrix.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[3][3] );
}

TEST(MatrixInitializeScalar)
{
    INVERTIBLE_MATRIX Matrix;
    ISTATUS Status;

    Status = MatrixInitializeScalar(&Matrix,
                                    (FLOAT) 1.0,
                                    (FLOAT) 2.0,
                                    (FLOAT) 3.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 3.0, Matrix.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix.Matrix.M[3][3] );
}

TEST(MatrixInitializeFrustum)
{

}

TEST(MatrixInitializeOrtho)
{

}

TEST(MatrixMultiply)
{
    INVERTIBLE_MATRIX Matrix1, Matrix2, Matrix3;
    ISTATUS Status;

    Status = MatrixInitializeTranslation(&Matrix1,
                                         (FLOAT) 1.0,
                                         (FLOAT) 2.0,
                                         (FLOAT) 3.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);    

    Status = MatrixInitializeScalar(&Matrix2,
                                    (FLOAT) 2.0,
                                    (FLOAT) 2.0,
                                    (FLOAT) 2.0);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Status = MatrixMultiply(&Matrix1, &Matrix2, &Matrix3);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    CHECK_EQUAL((FLOAT) 2.0, Matrix3.Matrix.M[0][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[0][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[0][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix3.Matrix.M[0][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[1][0] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix3.Matrix.M[1][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[1][2] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix3.Matrix.M[1][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[2][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[2][1] );
    CHECK_EQUAL((FLOAT) 2.0, Matrix3.Matrix.M[2][2] );
    CHECK_EQUAL((FLOAT) 3.0, Matrix3.Matrix.M[2][3] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[3][0] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[3][1] );
    CHECK_EQUAL((FLOAT) 0.0, Matrix3.Matrix.M[3][2] );
    CHECK_EQUAL((FLOAT) 1.0, Matrix3.Matrix.M[3][3] );
}