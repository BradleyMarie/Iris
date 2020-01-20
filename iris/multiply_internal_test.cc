/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    multiply_internal_test.cc

Abstract:

    Unit tests for multiply_internal.h

--*/

extern "C" {
#include "iris/matrix.h"
#include "iris/multiply_internal.h"
}

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(MultiplyInternalTest, VectorMatrixMultiplyInline)
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
    VECTOR3 actual = VectorMatrixMultiplyInline(m0, v0);

    VECTOR3 expected = 
        VectorCreate((float_t) 16.0, (float_t) 37.0, (float_t) 68.0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, VectorMatrixTransposedMultiplyInline)
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
    VECTOR3 actual = VectorMatrixTransposedMultiplyInline(m0, v0);

    VECTOR3 expected = 
        VectorCreate((float_t) 37.0, (float_t) 31.0, (float_t) 35.0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, VectorMatrixInverseMultiplyInline)
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
    VECTOR3 actual = VectorMatrixInverseMultiplyInline(m0, v0);

    VECTOR3 expected = VectorMatrixMultiplyInline(m0->inverse, v0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, VectorMatrixInverseTransposedMultiplyInline)
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
    VECTOR3 actual = VectorMatrixInverseTransposedMultiplyInline(m0, v0);

    VECTOR3 expected = VectorMatrixTransposedMultiplyInline(m0->inverse, v0);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, PointMatrixMultiplyInline)
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
    POINT3 actual = PointMatrixMultiplyInline(m0, p0);

    float_t inverse_w = (float_t) 1.0 / (float_t) 125.0;
    POINT3 expected = PointCreate((float_t) 20.0 * inverse_w, 
                                  (float_t) 45.0 * inverse_w, 
                                  (float_t) 80.0 * inverse_w);
    
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, PointMatrixInverseMultiplyInline)
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
    POINT3 actual = PointMatrixInverseMultiplyInline(m0, p0);

    POINT3 expected =  PointMatrixMultiplyInline(m0->inverse, p0);
    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}

TEST(MultiplyInternalTest, RayMatrixMultiplyInline)
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
    RAY actual = RayMatrixMultiplyInline(m0, ray);

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

TEST(MultiplyInternalTest, RayMatrixInverseMultiplyInline)
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
    RAY actual = RayMatrixInverseMultiplyInline(m0, ray);

    RAY expected = RayMatrixMultiplyInline(m0->inverse, ray);

    EXPECT_EQ(expected, actual);
    MatrixRelease(m0);
}