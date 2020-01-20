/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    multiply_test.cc

Abstract:

    Unit tests for multiply.c

--*/

extern "C" {
#include "iris/multiply.h"
}

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(MultiplyTest, VectorMatrixMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixMultiply(nullptr, v0);

    EXPECT_EQ(v0, actual);
}

TEST(MultiplyTest, VectorMatrixMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixMultiply(m0, v0);

    VECTOR3 expected = 
        VectorCreate((float_t) 16.0, (float_t) 37.0, (float_t) 68.0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, VectorMatrixTransposedMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixTransposedMultiply(nullptr, v0);

    EXPECT_EQ(v0, actual);
}

TEST(MultiplyTest, VectorMatrixTransposedMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixTransposedMultiply(m0, v0);

    VECTOR3 expected = 
        VectorCreate((float_t) 37.0, (float_t) 31.0, (float_t) 35.0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, VectorMatrixInverseMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixInverseMultiply(nullptr, v0);

    EXPECT_EQ(v0, actual);
}

TEST(MultiplyTest, VectorMatrixInverseMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixInverseMultiply(m0, v0);

    PCMATRIX inverse = MatrixGetConstantInverse(m0);
    VECTOR3 expected = VectorMatrixMultiply(inverse, v0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, VectorMatrixInverseTransposedMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixInverseTransposedMultiply(nullptr, v0);

    EXPECT_EQ(v0, actual);
}

TEST(MultiplyTest, VectorMatrixInverseTransposedMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    VECTOR3 actual = VectorMatrixInverseTransposedMultiply(m0, v0);

    PCMATRIX inverse = MatrixGetConstantInverse(m0);
    VECTOR3 expected = VectorMatrixTransposedMultiply(inverse, v0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, PointMatrixMultiplyNull)
{
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 actual = PointMatrixMultiply(nullptr, p0);

    EXPECT_EQ(p0, actual);
}

TEST(MultiplyTest, PointMatrixMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 actual = PointMatrixMultiply(m0, p0);

    float_t inverse_w = (float_t) 1.0 / (float_t) 125.0;
    POINT3 expected = PointCreate((float_t) 20.0 * inverse_w, 
                                  (float_t) 45.0 * inverse_w, 
                                  (float_t) 80.0 * inverse_w);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, PointMatrixInverseMultiplyNull)
{
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 actual = PointMatrixInverseMultiply(nullptr, p0);

    EXPECT_EQ(p0, actual);
}

TEST(MultiplyTest, PointMatrixInverseMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 actual = PointMatrixInverseMultiply(m0, p0);

    PCMATRIX inverse = MatrixGetConstantInverse(m0);
    POINT3 expected =  PointMatrixMultiply(inverse, p0);
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, RayMatrixMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);
    RAY actual = RayMatrixMultiply(nullptr, ray);

    EXPECT_EQ(ray, actual);
}

TEST(MultiplyTest, RayMatrixMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);
    RAY actual = RayMatrixMultiply(m0, ray);

    float_t inverse_w = (float_t) 1.0 / (float_t) 125.0;
    POINT3 expected_origin = PointCreate((float_t) 20.0 * inverse_w, 
                                         (float_t) 45.0 * inverse_w, 
                                         (float_t) 80.0 * inverse_w);
    VECTOR3 expected_direction = VectorCreate(
        (float_t) 16.0, (float_t) 37.0, (float_t) 68.0);
    RAY expected = RayCreate(expected_origin, expected_direction);

    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyTest, RayMatrixInverseMultiplyNull)
{
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);
    RAY actual = RayMatrixInverseMultiply(nullptr, ray);

    EXPECT_EQ(ray, actual);
}

TEST(MultiplyTest, RayMatrixInverseMultiply)
{
    PMATRIX m0;
    ISTATUS status = MatrixAllocate(
        (float_t) 1.0, (float_t) 2.0, (float_t) 3.0, (float_t) 4.0,
        (float_t) 5.0, (float_t) 1.0, (float_t) 7.0, (float_t) 8.0,
        (float_t) 9.0, (float_t) 10.0, (float_t) 1.0, (float_t) 12.0,
        (float_t) 13.0, (float_t) 14.0, (float_t) 15.0, (float_t) 1.0,
        &m0);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);
    RAY actual = RayMatrixInverseMultiply(m0, ray);

    PCMATRIX inverse = MatrixGetConstantInverse(m0);
    RAY expected = RayMatrixMultiply(inverse, ray);

    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}