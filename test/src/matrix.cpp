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
    FLOAT Contents[4][4];
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocate((FLOAT) 1.0,
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
                            (FLOAT) 16.0,
                            &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix, Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 4.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 5.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 6.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 7.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 8.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 9.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 10.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 11.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 12.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 13.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 14.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 15.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 16.0, Contents[3][3]);
}

TEST(MatrixInverse)
{
    FLOAT Contents[4][4];
    FLOAT InverseContents[4][4];
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocate((FLOAT) 1.0,
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
                            (FLOAT) 1.0,
                            &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix, Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);

    Matrix = MatrixGetInverse(Matrix);

    MatrixReadContents(Matrix, InverseContents);

    CHECK_EQUAL((FLOAT) 1.0, InverseContents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[0][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[1][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[2][2]);
    CHECK_EQUAL((FLOAT) -1.0, InverseContents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, InverseContents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, InverseContents[3][3]);
}

TEST(MatrixInitializeTranslation)
{
    FLOAT Contents[4][4];
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 2.0,
                                       (FLOAT) 3.0,
                                       &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix, Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixInitializeScalar)
{
    FLOAT Contents[4][4];
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocateScalar((FLOAT) 1.0,
                                  (FLOAT) 2.0,
                                  (FLOAT) 3.0,
                                  &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix, Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixInitializeFrustum)
{

}

TEST(MatrixInitializeOrtho)
{

}

TEST(MatrixMultiply)
{
    FLOAT Contents[4][4];
    PMATRIX Matrix1, Matrix2, Matrix3;
    ISTATUS Status;

    Status = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 2.0,
                                       (FLOAT) 3.0,
                                       &Matrix1);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Status = MatrixAllocateScalar((FLOAT) 2.0,
                                  (FLOAT) 2.0,
                                  (FLOAT) 2.0,
                                  &Matrix2);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Status = MatrixAllocateProduct(Matrix1, Matrix2, &Matrix3);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix3, Contents);

    CHECK_EQUAL((FLOAT) 2.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}

TEST(MatrixMultiplyWithIdentity)
{
    FLOAT Contents[4][4];
    PMATRIX Matrix;
    ISTATUS Status;

    Status = MatrixAllocateTranslation((FLOAT) 1.0,
                                       (FLOAT) 2.0,
                                       (FLOAT) 3.0,
                                       &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Status = MatrixAllocateProduct(NULL, Matrix, &Matrix);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    MatrixReadContents(Matrix, Contents);

    CHECK_EQUAL((FLOAT) 1.0, Contents[0][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[0][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[0][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][0]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[1][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[1][2]);
    CHECK_EQUAL((FLOAT) 2.0, Contents[1][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[2][1]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[2][2]);
    CHECK_EQUAL((FLOAT) 3.0, Contents[2][3]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][0]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][1]);
    CHECK_EQUAL((FLOAT) 0.0, Contents[3][2]);
    CHECK_EQUAL((FLOAT) 1.0, Contents[3][3]);
}