/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    matrix_test.cc

Abstract:

    Unit tests for matrix.c

--*/

extern "C" {
#include "iris/matrix.h"
}

#include <functional>
#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(MatrixTest, MatrixAllocateArgumentErrors)
{
    float_t contents[4][4] = {
        { (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0 },
        { (float_t) 0.0, (float_t) 1.0, (float_t) 0.0, (float_t) 0.0 },
        { (float_t) 0.0, (float_t) 0.0, (float_t) 1.0, (float_t) 0.0 },
        { (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0 }
    };

    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocate(
                contents[0][0], contents[0][1], contents[0][2], contents[0][3],
                contents[1][0], contents[1][1], contents[1][2], contents[1][3],
                contents[2][0], contents[2][1], contents[2][2], contents[2][3],
                contents[3][0], contents[3][1], contents[3][2], contents[3][3],
                output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, int, ISTATUS)> 
        do_permutation = [&](int i, int j, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = contents[i][j];
            
            contents[i][j] = INFINITY;
            do_allocate(&matrix, expected);
            
            contents[i][j] = -INFINITY;
            do_allocate(&matrix, expected);
            
            contents[i][j] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            contents[i][j] = original_value;
    };

    do_permutation(0, 0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(0, 1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(0, 2, ISTATUS_INVALID_ARGUMENT_02);
    do_permutation(0, 3, ISTATUS_INVALID_ARGUMENT_03);
    do_permutation(1, 0, ISTATUS_INVALID_ARGUMENT_04);
    do_permutation(1, 1, ISTATUS_INVALID_ARGUMENT_05);
    do_permutation(1, 2, ISTATUS_INVALID_ARGUMENT_06);
    do_permutation(1, 3, ISTATUS_INVALID_ARGUMENT_07);
    do_permutation(2, 0, ISTATUS_INVALID_ARGUMENT_08);
    do_permutation(2, 1, ISTATUS_INVALID_ARGUMENT_09);
    do_permutation(2, 2, ISTATUS_INVALID_ARGUMENT_10);
    do_permutation(2, 3, ISTATUS_INVALID_ARGUMENT_11);
    do_permutation(3, 0, ISTATUS_INVALID_ARGUMENT_12);
    do_permutation(3, 1, ISTATUS_INVALID_ARGUMENT_13);
    do_permutation(3, 2, ISTATUS_INVALID_ARGUMENT_14);
    do_permutation(3, 3, ISTATUS_INVALID_ARGUMENT_15);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_16);
}

TEST(MatrixTest, MatrixAllocateNotInvertibleError)
{
    PMATRIX matrix;
    ISTATUS actual = MatrixAllocate(
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        &matrix);
    EXPECT_EQ(ISTATUS_ARITHMETIC_ERROR, actual);
} 

TEST(MatrixTest, MatrixAllocate)
{
    PMATRIX matrix;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    float_t contents[4][4];
    MatrixReadContents(matrix, contents);
    EXPECT_EQ((float_t) 1.0, contents[0][0]);
    EXPECT_EQ((float_t) 2.0, contents[0][1]);
    EXPECT_EQ((float_t) 3.0, contents[0][2]);
    EXPECT_EQ((float_t) 4.0, contents[0][3]);
    EXPECT_EQ((float_t) 5.0, contents[1][0]);
    EXPECT_EQ((float_t) 1.0, contents[1][1]);
    EXPECT_EQ((float_t) 7.0, contents[1][2]);
    EXPECT_EQ((float_t) 8.0, contents[1][3]);
    EXPECT_EQ((float_t) 9.0, contents[2][0]);
    EXPECT_EQ((float_t) 10.0, contents[2][1]);
    EXPECT_EQ((float_t) 1.0, contents[2][2]);
    EXPECT_EQ((float_t) 12.0, contents[2][3]);
    EXPECT_EQ((float_t) 13.0, contents[3][0]);
    EXPECT_EQ((float_t) 14.0, contents[3][1]);
    EXPECT_EQ((float_t) 15.0, contents[3][2]);
    EXPECT_EQ((float_t) 1.0, contents[3][3]);

    PCMATRIX inverse = MatrixGetConstantInverse(matrix);

    MatrixReadContents(inverse, contents);
    EXPECT_NEAR((float_t) -0.463218, contents[0][0], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.151724, contents[0][1], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.051724, contents[0][2], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.018391, contents[0][3], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.255172, contents[1][0], (float_t) 0.0001);
    EXPECT_NEAR((float_t) -0.172414, contents[1][1], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[1][2], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[1][3], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.155172, contents[2][0], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[2][1], (float_t) 0.0001);
    EXPECT_NEAR((float_t) -0.072414, contents[2][2], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[2][3], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.121839, contents[3][0], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[3][1], (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.027586, contents[3][2], (float_t) 0.0001);
    EXPECT_NEAR((float_t) -0.039080, contents[3][3], (float_t) 0.0001);

    MatrixRelease(matrix);
}

TEST(MatrixTest, MatrixAllocateTranslationArgumentErrors)
{
    float_t params[3] = { (float_t) 1.0, (float_t) 2.0, (float_t) 3.0 };
    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocateTranslation(
                params[0], params[1], params[2], output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, ISTATUS)> 
        do_permutation = [&](int i, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = params[i];
            
            params[i] = INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = -INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            params[i] = original_value;
    };

    do_permutation(0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(2, ISTATUS_INVALID_ARGUMENT_02);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_03);
}

TEST(MatrixTest, MatrixAllocateTranslation)
{
    PMATRIX actual;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        (float_t) 0.0, (float_t) 1.0, (float_t) 0.0, (float_t) 2.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 1.0, (float_t) 3.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, EqualsMatrix(actual));

    MatrixRelease(expected);
    MatrixRelease(actual);
}

TEST(MatrixTest, MatrixAllocateScalarArgumentErrors)
{
    float_t params[3] = { (float_t) 1.0, (float_t) 2.0, (float_t) 3.0 };
    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocateScalar(
                params[0], params[1], params[2], output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, ISTATUS)> 
        do_permutation = [&](int i, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = params[i];
            
            params[i] = INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = (float_t) 0.0;
            do_allocate(&matrix, expected);
            
            params[i] = (float_t) -0.0;
            do_allocate(&matrix, expected);
            
            params[i] = -INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            params[i] = original_value;
    };

    do_permutation(0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(2, ISTATUS_INVALID_ARGUMENT_02);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_03);
}

TEST(MatrixTest, MatrixAllocateScalar)
{
    PMATRIX actual;
    ISTATUS status = MatrixAllocateScalar((float_t) 1.0,
                                          (float_t) 2.0,
                                          (float_t) 3.0,
                                          &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 2.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 3.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, EqualsMatrix(actual));
    
    MatrixRelease(expected);
    MatrixRelease(actual);
}

TEST(MatrixTest, MatrixAllocateRotationArgumentErrors)
{
    float_t params[4] = { 
        (float_t) 0.0, (float_t) 1.0, (float_t) 2.0, (float_t) 3.0
    };

    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocateRotation(
                params[0], params[1], params[2], params[3], output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, ISTATUS)> 
        do_permutation = [&](int i, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = params[i];
            
            params[i] = INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = -INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            params[i] = original_value;
    };

    do_permutation(0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(2, ISTATUS_INVALID_ARGUMENT_02);
    do_permutation(3, ISTATUS_INVALID_ARGUMENT_03);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_04);

    PMATRIX matrix;
    ISTATUS actual = MatrixAllocateRotation(
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, actual);

    actual = MatrixAllocateRotation(
        (float_t) 0.0, (float_t) -0.0, (float_t) -0.0, (float_t) -0.0, &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, actual);
}

TEST(MatrixTest, MatrixAllocateRotation)
{
    PMATRIX actual;
    ISTATUS status = MatrixAllocateRotation((float_t) 4.0,
                                            (float_t) 1.0,
                                            (float_t) 2.0,
                                            (float_t) 3.0,
                                            &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t zero = (float_t) 0.0;
    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) -0.5355262, (float_t) 0.8430267, (float_t) -0.0501757, zero,
        (float_t) -0.3705571, (float_t) -0.181174, (float_t) 0.9109684, zero,
        (float_t) 0.7588801, (float_t) 0.5064405, (float_t) 0.4094130, zero,
        zero, zero, zero, (float_t) 1.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, ApproximatelyEqualsMatrix(actual, (float_t) 0.0001));
    
    MatrixRelease(expected);
    MatrixRelease(actual);
}

TEST(MatrixTest, MatrixAllocateOrthographicArgumentErrors)
{
    float_t params[6] = { 
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 3.0, (float_t) 4.0, 
        (float_t) 5.0, (float_t) 6.0,
    };

    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocateOrthographic(
                params[0], params[1], 
                params[2], params[3],
                params[4], params[5], 
                output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, ISTATUS)> 
        do_permutation = [&](int i, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = params[i];
            
            params[i] = INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = -INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            params[i] = original_value;
    };

    do_permutation(0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(2, ISTATUS_INVALID_ARGUMENT_02);
    do_permutation(3, ISTATUS_INVALID_ARGUMENT_03);
    do_permutation(4, ISTATUS_INVALID_ARGUMENT_04);
    do_permutation(5, ISTATUS_INVALID_ARGUMENT_05);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_06);

    PMATRIX matrix;
    ISTATUS actual = MatrixAllocateOrthographic(
        (float_t) 1.0, (float_t) 1.0, 
        (float_t) 2.0, (float_t) 3.0, 
        (float_t) 4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, actual);

    actual = MatrixAllocateOrthographic(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 2.0, (float_t) 2.0, 
        (float_t) 4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, actual);

    actual = MatrixAllocateOrthographic(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 2.0, (float_t) 3.0, 
        (float_t) 4.0, (float_t) 4.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, actual);
}

TEST(MatrixTest, MatrixAllocateOrthographic)
{
    PMATRIX actual;
    ISTATUS status = MatrixAllocateOrthographic(
        (float_t) 1.0, (float_t) -3.0, 
        (float_t) 2.0, (float_t) -3.0,
        (float_t) 7.0, (float_t) -3.0,
        &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) -0.5, (float_t) 0.0, (float_t) 0.0, (float_t) -0.5,
        (float_t) 0.0, (float_t) -0.4, (float_t) 0.0, (float_t) -0.2,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.2, (float_t) 0.4,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, ApproximatelyEqualsMatrix(actual, (float_t) 0.0001));
    
    MatrixRelease(expected);
    MatrixRelease(actual);
}

TEST(MatrixTest, MatrixAllocateFrustumArgumentErrors)
{
    float_t params[6] = { 
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 3.0, (float_t) 4.0, 
        (float_t) 5.0, (float_t) 6.0,
    };

    std::function<void(PMATRIX *, ISTATUS)> 
        do_allocate = [&](PMATRIX *output, ISTATUS expected) {
            ISTATUS actual = MatrixAllocateFrustum(
                params[0], params[1], 
                params[2], params[3],
                params[4], params[5], 
                output);
            EXPECT_EQ(expected, actual);
    };

    std::function<void(int, ISTATUS)> 
        do_permutation = [&](int i, ISTATUS expected) {
            PMATRIX matrix;
            float_t original_value = params[i];
            
            params[i] = INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = -INFINITY;
            do_allocate(&matrix, expected);
            
            params[i] = std::numeric_limits<float_t>::quiet_NaN();
            do_allocate(&matrix, expected);

            params[i] = original_value;
    };

    do_permutation(0, ISTATUS_INVALID_ARGUMENT_00);
    do_permutation(1, ISTATUS_INVALID_ARGUMENT_01);
    do_permutation(2, ISTATUS_INVALID_ARGUMENT_02);
    do_permutation(3, ISTATUS_INVALID_ARGUMENT_03);
    do_permutation(4, ISTATUS_INVALID_ARGUMENT_04);
    do_permutation(5, ISTATUS_INVALID_ARGUMENT_05);
    do_allocate(nullptr, ISTATUS_INVALID_ARGUMENT_06);

    PMATRIX matrix;
    ISTATUS actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 3.0, (float_t) 4.0, 
        (float_t) -4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, actual);

    actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 3.0, (float_t) 4.0, 
        (float_t) 4.0, (float_t) -5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, actual);

    actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 1.0, 
        (float_t) 2.0, (float_t) 3.0, 
        (float_t) 4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, actual);

    actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 1.0, 
        (float_t) 2.0, (float_t) 3.0, 
        (float_t) 4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, actual);

    actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 2.0, (float_t) 2.0, 
        (float_t) 4.0, (float_t) 5.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, actual);

    actual = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) 2.0, 
        (float_t) 2.0, (float_t) 3.0, 
        (float_t) 4.0, (float_t) 4.0, 
        &matrix);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, actual);
}

TEST(MatrixTest, MatrixAllocateFrustum)
{
    PMATRIX actual;
    ISTATUS status = MatrixAllocateFrustum(
        (float_t) 1.0, (float_t) -3.0, 
        (float_t) 2.0, (float_t) -3.0,
        (float_t) 7.0, (float_t) 3.0,
        &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) -3.5, (float_t) 0.0, (float_t) 0.5, (float_t) 0.0,
        (float_t) 0.0, (float_t) -2.8, (float_t) 0.2, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 2.5, (float_t) 10.5,
        (float_t) 0.0, (float_t) 0.0, (float_t) -1.0, (float_t) 0.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, ApproximatelyEqualsMatrix(actual, (float_t) 0.0001));
    
    MatrixRelease(expected);
    MatrixRelease(actual);
}

TEST(MatrixTest, MatrixMultiplyArgumentError)
{
    PMATRIX matrix;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, 
              MatrixAllocateProduct(matrix, matrix, nullptr));
    
    MatrixRelease(matrix);
}

TEST(MatrixTest, MatrixMultiplyNull)
{
    PMATRIX matrix;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX actual;
    status = MatrixAllocateProduct(nullptr, nullptr, &actual);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(nullptr, actual);
    MatrixRelease(actual);

    status = MatrixAllocateProduct(matrix, nullptr, &actual);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(matrix, actual);
    MatrixRelease(actual);

    status = MatrixAllocateProduct(nullptr, matrix, &actual);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(matrix, actual);
    MatrixRelease(actual);

    MatrixRelease(matrix);
}

TEST(MatrixTest, MatrixMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX m1;
    status = MatrixAllocate(
        (float_t) 2.0, (float_t) 3.0, (float_t) 4.0, (float_t) 5.0,
        (float_t) 5.0, (float_t) 2.0, (float_t) 18.0, (float_t) 7.0,
        (float_t) 5.0, (float_t) 10.0, (float_t) 4.0, (float_t) 12.0,
        (float_t) 7.0, (float_t) 4.0, (float_t) 1.0, (float_t) 1.0,
        &m1);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX actual;
    status = MatrixAllocateProduct(m0, m1, &actual);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX expected;
    status = MatrixAllocate(
        (float_t) 55.0, (float_t) 53.0, (float_t) 56.0, (float_t) 59.0,
        (float_t) 106.0, (float_t) 119.0, (float_t) 74.0, (float_t) 124.0,
        (float_t) 157.0, (float_t) 105.0, (float_t) 232.0, (float_t) 139.0,
        (float_t) 178.0, (float_t) 221.0, (float_t) 365.0, (float_t) 344.0,
        &expected);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_THAT(expected, ApproximatelyEqualsMatrix(actual, (float_t) 0.001));
    MatrixRelease(m0);
    MatrixRelease(m1);
    MatrixRelease(actual);
    MatrixRelease(expected);
}

TEST(MatrixTest, MatrixGetInverse)
{
    PMATRIX base_matrix;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &base_matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX inverse = MatrixGetInverse(base_matrix);
    MatrixRelease(base_matrix);

    PMATRIX new_matrix = MatrixGetInverse(inverse);
    MatrixRelease(inverse);

    ASSERT_EQ(base_matrix, new_matrix);
    MatrixRelease(new_matrix);
}

TEST(MatrixTest, MatrixGetInverseNull)
{
    ASSERT_EQ(nullptr, MatrixGetInverse(nullptr));
}

TEST(MatrixTest, MatrixGetConstantInverse)
{
    PMATRIX expected;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 0.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 1.0, (float_t) 0.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 1.0, (float_t) 0.0,
        (float_t) 0.0, (float_t) 0.0, (float_t) 0.0, (float_t) 1.0,
        &expected);

    ASSERT_EQ(ISTATUS_SUCCESS, status);
    PCMATRIX inverse = MatrixGetConstantInverse(expected);
    EXPECT_NE(expected, inverse);
    PCMATRIX actual = MatrixGetConstantInverse(inverse);
    EXPECT_EQ(expected, actual);

    MatrixRelease(expected);
}

TEST(MatrixTest, MatrixGetConstantInverseNull)
{
    ASSERT_EQ(nullptr, MatrixGetConstantInverse(nullptr));
}

TEST(MatrixTest, MatrixReadContentsArgumentError)
{
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_01, 
              MatrixReadContents(nullptr, nullptr));
}

TEST(MatrixTest, MatrixReadContentsNull)
{
    float_t contents[4][4];
    ISTATUS status = MatrixReadContents(nullptr, contents);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_EQ((float_t) 1.0, contents[0][0]);
    EXPECT_EQ((float_t) 0.0, contents[0][1]);
    EXPECT_EQ((float_t) 0.0, contents[0][2]);
    EXPECT_EQ((float_t) 0.0, contents[0][3]);
    EXPECT_EQ((float_t) 0.0, contents[1][0]);
    EXPECT_EQ((float_t) 1.0, contents[1][1]);
    EXPECT_EQ((float_t) 0.0, contents[1][2]);
    EXPECT_EQ((float_t) 0.0, contents[1][3]);
    EXPECT_EQ((float_t) 0.0, contents[2][0]);
    EXPECT_EQ((float_t) 0.0, contents[2][1]);
    EXPECT_EQ((float_t) 1.0, contents[2][2]);
    EXPECT_EQ((float_t) 0.0, contents[2][3]);
    EXPECT_EQ((float_t) 0.0, contents[3][0]);
    EXPECT_EQ((float_t) 0.0, contents[3][1]);
    EXPECT_EQ((float_t) 0.0, contents[3][2]);
    EXPECT_EQ((float_t) 1.0, contents[3][3]);
}

TEST(MatrixTest, MatrixReadContents)
{
    PMATRIX matrix;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    float_t contents[4][4];
    MatrixReadContents(matrix, contents);
    EXPECT_EQ((float_t) 1.0, contents[0][0]);
    EXPECT_EQ((float_t) 2.0, contents[0][1]);
    EXPECT_EQ((float_t) 3.0, contents[0][2]);
    EXPECT_EQ((float_t) 4.0, contents[0][3]);
    EXPECT_EQ((float_t) 5.0, contents[1][0]);
    EXPECT_EQ((float_t) 1.0, contents[1][1]);
    EXPECT_EQ((float_t) 7.0, contents[1][2]);
    EXPECT_EQ((float_t) 8.0, contents[1][3]);
    EXPECT_EQ((float_t) 9.0, contents[2][0]);
    EXPECT_EQ((float_t) 10.0, contents[2][1]);
    EXPECT_EQ((float_t) 1.0, contents[2][2]);
    EXPECT_EQ((float_t) 12.0, contents[2][3]);
    EXPECT_EQ((float_t) 13.0, contents[3][0]);
    EXPECT_EQ((float_t) 14.0, contents[3][1]);
    EXPECT_EQ((float_t) 15.0, contents[3][2]);
    EXPECT_EQ((float_t) 1.0, contents[3][3]);
}