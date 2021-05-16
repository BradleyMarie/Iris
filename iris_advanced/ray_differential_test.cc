/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    ray_differential_test.cc

Abstract:

    Unit tests for ray_differential.h

--*/

extern "C" {
#include "iris_advanced/ray_differential.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(RayDifferentialTest, RayDifferentialTestCreate)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    POINT3 point1 = PointCreate(3.0f, 4.0f, 5.0f);
    POINT3 point2 = PointCreate(6.0f, 7.0f, 8.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    VECTOR3 vector1 = VectorCreate(12.0f, 13.0f, 14.0f);
    VECTOR3 vector2 = VectorCreate(15.0f, 16.0f, 17.0f);
    RAY ray0 = RayCreate(point0, vector0);
    RAY ray1 = RayCreate(point1, vector1);
    RAY ray2 = RayCreate(point2, vector2);

    RAY_DIFFERENTIAL differential = RayDifferentialCreate(ray0, ray1, ray2);
    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray1, differential.rx);
    EXPECT_EQ(ray2, differential.ry);
    EXPECT_TRUE(differential.has_differentials);
}

TEST(RayDifferentialTest, RayDifferentialTestCreateWithoutDifferentials)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    RAY ray0 = RayCreate(point0, vector0);

    RAY_DIFFERENTIAL differential =
        RayDifferentialCreateWithoutDifferentials(ray0);
    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray0, differential.rx);
    EXPECT_EQ(ray0, differential.ry);
    EXPECT_FALSE(differential.has_differentials);
}

TEST(RayDifferentialTest, RayDifferentialTestValidate)
{
    POINT3 point = PointCreate(1.0f, 2.0f, 3.0f);
    VECTOR3 vector = VectorCreate(9.0f, 10.0f, 11.0f);
    RAY ray = RayCreate(point, vector);

    RAY_DIFFERENTIAL differential =
        RayDifferentialCreateWithoutDifferentials(ray);
    EXPECT_TRUE(RayDifferentialValidate(differential));

    differential.ray.origin.x = INFINITY;
    EXPECT_FALSE(RayDifferentialValidate(differential));
    differential.ray.origin.x = 0.0f;

    differential.rx.origin.x = INFINITY;
    EXPECT_FALSE(RayDifferentialValidate(differential));
    differential.rx.origin.x = 0.0f;

    differential.ry.origin.x = INFINITY;
    EXPECT_FALSE(RayDifferentialValidate(differential));
}

TEST(RayDifferentialTest, RayDifferentialTestMultiply)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    POINT3 point1 = PointCreate(3.0f, 4.0f, 5.0f);
    POINT3 point2 = PointCreate(6.0f, 7.0f, 8.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    VECTOR3 vector1 = VectorCreate(12.0f, 13.0f, 14.0f);
    VECTOR3 vector2 = VectorCreate(15.0f, 16.0f, 17.0f);
    RAY ray0 = RayCreate(point0, vector0);
    RAY ray1 = RayCreate(point1, vector1);
    RAY ray2 = RayCreate(point2, vector2);

    PMATRIX matrix;
    ISTATUS status = MatrixAllocateScalar(2.0f, 2.0f, 2.0f, &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    RAY_DIFFERENTIAL differential = RayDifferentialCreate(ray0, ray1, ray2);
    differential = RayDifferentialMatrixMultiply(matrix, differential);

    ray0 = RayMatrixMultiply(matrix, ray0);
    ray1 = RayMatrixMultiply(matrix, ray1);
    ray2 = RayMatrixMultiply(matrix, ray2);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray1, differential.rx);
    EXPECT_EQ(ray2, differential.ry);
    EXPECT_TRUE(differential.has_differentials);

    MatrixRelease(matrix);
}

TEST(RayDifferentialTest, RayDifferentialTestMultiplyNoDifferentials)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    RAY ray0 = RayCreate(point0, vector0);

    PMATRIX matrix;
    ISTATUS status = MatrixAllocateScalar(2.0f, 2.0f, 2.0f, &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    RAY_DIFFERENTIAL differential =
        RayDifferentialCreateWithoutDifferentials(ray0);
    differential = RayDifferentialMatrixMultiply(matrix, differential);

    ray0 = RayMatrixMultiply(matrix, ray0);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray0, differential.rx);
    EXPECT_EQ(ray0, differential.ry);
    EXPECT_FALSE(differential.has_differentials);

    MatrixRelease(matrix);
}

TEST(RayDifferentialTest, RayDifferentialTestInverseMultiply)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    POINT3 point1 = PointCreate(3.0f, 4.0f, 5.0f);
    POINT3 point2 = PointCreate(6.0f, 7.0f, 8.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    VECTOR3 vector1 = VectorCreate(12.0f, 13.0f, 14.0f);
    VECTOR3 vector2 = VectorCreate(15.0f, 16.0f, 17.0f);
    RAY ray0 = RayCreate(point0, vector0);
    RAY ray1 = RayCreate(point1, vector1);
    RAY ray2 = RayCreate(point2, vector2);

    PMATRIX matrix;
    ISTATUS status = MatrixAllocateScalar(2.0f, 2.0f, 2.0f, &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    RAY_DIFFERENTIAL differential = RayDifferentialCreate(ray0, ray1, ray2);
    differential = RayDifferentialMatrixInverseMultiply(matrix, differential);

    ray0 = RayMatrixInverseMultiply(matrix, ray0);
    ray1 = RayMatrixInverseMultiply(matrix, ray1);
    ray2 = RayMatrixInverseMultiply(matrix, ray2);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray1, differential.rx);
    EXPECT_EQ(ray2, differential.ry);
    EXPECT_TRUE(differential.has_differentials);

    MatrixRelease(matrix);
}

TEST(RayDifferentialTest, RayDifferentialTestInverseMultiplyNoDifferentials)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    VECTOR3 vector0 = VectorCreate(9.0f, 10.0f, 11.0f);
    RAY ray0 = RayCreate(point0, vector0);

    PMATRIX matrix;
    ISTATUS status = MatrixAllocateScalar(2.0f, 2.0f, 2.0f, &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    RAY_DIFFERENTIAL differential =
        RayDifferentialCreateWithoutDifferentials(ray0);
    differential = RayDifferentialMatrixInverseMultiply(matrix, differential);

    ray0 = RayMatrixInverseMultiply(matrix, ray0);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray0, differential.rx);
    EXPECT_EQ(ray0, differential.ry);
    EXPECT_FALSE(differential.has_differentials);

    MatrixRelease(matrix);
}

TEST(RayDifferentialTest, RayDifferentialTestNormalize)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    POINT3 point1 = PointCreate(3.0f, 4.0f, 5.0f);
    POINT3 point2 = PointCreate(6.0f, 7.0f, 8.0f);
    VECTOR3 vector0 = VectorCreate(2.0f, 0.0f, 0.0f);
    VECTOR3 vector1 = VectorCreate(0.0f, 2.0f, 0.0f);
    VECTOR3 vector2 = VectorCreate(0.0f, 0.0f, 2.0f);
    RAY ray0 = RayCreate(point0, vector0);
    RAY ray1 = RayCreate(point1, vector1);
    RAY ray2 = RayCreate(point2, vector2);

    RAY_DIFFERENTIAL differential = RayDifferentialCreate(ray0, ray1, ray2);
    differential = RayDifferentialNormalize(differential);

    ray0.direction = VectorNormalize(ray0.direction, NULL, NULL);
    ray1.direction = VectorNormalize(ray1.direction, NULL, NULL);
    ray2.direction = VectorNormalize(ray2.direction, NULL, NULL);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray1, differential.rx);
    EXPECT_EQ(ray2, differential.ry);
    EXPECT_TRUE(differential.has_differentials);
}

TEST(RayDifferentialTest, RayDifferentialTestNormalizeNoDifferentials)
{
    POINT3 point0 = PointCreate(1.0f, 2.0f, 3.0f);
    VECTOR3 vector0 = VectorCreate(2.0f, 0.0f, 0.0f);
    RAY ray0 = RayCreate(point0, vector0);

    RAY_DIFFERENTIAL differential =
        RayDifferentialCreateWithoutDifferentials(ray0);
    differential = RayDifferentialNormalize(differential);

    ray0.direction = VectorNormalize(ray0.direction, NULL, NULL);

    EXPECT_EQ(ray0, differential.ray);
    EXPECT_EQ(ray0, differential.rx);
    EXPECT_EQ(ray0, differential.ry);
    EXPECT_FALSE(differential.has_differentials);
}