/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    intersection_test.cc

Abstract:

    Unit tests for intersection.h

--*/

extern "C" {
#include "iris_advanced/intersection.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"

RAY_DIFFERENTIAL
CreateWorldRay(
    void
    )
{
    POINT3 origin = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    VECTOR3 direction = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    VECTOR3 rx = VectorCreate((float_t)0.5, (float_t)0.0, (float_t)0.5);
    rx = VectorNormalize(rx, NULL, NULL);
    VECTOR3 ry = VectorCreate((float_t)0.0, (float_t)0.5, (float_t)0.5);
    ry = VectorNormalize(ry, NULL, NULL);

    RAY ray0 = RayCreate(origin, direction);
    RAY ray1 = RayCreate(origin, rx);
    RAY ray2 = RayCreate(origin, ry);

    return RayDifferentialCreate(ray0, ray1, ray2);
}

POINT3
CreateModelHitPoint(
    void
    )
{
    return PointCreate((float_t)0.0, (float_t)0.0, (float_t)-2.0);
}

POINT3
CreateWorldHitPoint(
    void
    )
{
    return PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
}

TEST(IntersectionTest, IntersectionCreate)
{
    PMATRIX matrix;
    ISTATUS status =  MatrixAllocateScalar((float_t)2.0,
                                           (float_t)2.0,
                                           (float_t)-2.0,
                                           &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    INTERSECTION actual = IntersectionCreate(CreateWorldRay(),
                                             matrix,
                                             CreateModelHitPoint(),
                                             CreateWorldHitPoint(),
                                             normal);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_TRUE(actual.has_derivatives);
    EXPECT_NEAR(actual.model_dp_dx.x, (float_t)2.0, (float_t)0.0001);
    EXPECT_NEAR(actual.model_dp_dx.y, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.model_dp_dx.z, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.model_dp_dy.x, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.model_dp_dy.y, (float_t)2.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dy.z, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dx.x, (float_t)1.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dx.y, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dx.z, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dy.x, (float_t)0.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dy.y, (float_t)1.0, (float_t)0.0001);
    EXPECT_NEAR(actual.world_dp_dy.z, (float_t)0.0, (float_t)0.0001);

    MatrixRelease(matrix);
}

TEST(IntersectionTest, IntersectionCreateNoDifferentials)
{
    PMATRIX matrix;
    ISTATUS status =  MatrixAllocateScalar((float_t)2.0,
                                           (float_t)2.0,
                                           (float_t)-2.0,
                                           &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    RAY_DIFFERENTIAL world_ray = CreateWorldRay();
    world_ray.has_differentials = false;

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    INTERSECTION actual = IntersectionCreate(world_ray,
                                             matrix,
                                             CreateModelHitPoint(),
                                             CreateWorldHitPoint(),
                                             normal);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);

    MatrixRelease(matrix);
}

TEST(IntersectionTest, IntersectionCreateInfiniteTx)
{
    PMATRIX matrix;
    ISTATUS status =  MatrixAllocateScalar((float_t)2.0,
                                           (float_t)2.0,
                                           (float_t)-2.0,
                                           &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0);
    INTERSECTION actual = IntersectionCreate(CreateWorldRay(),
                                             matrix,
                                             CreateModelHitPoint(),
                                             CreateWorldHitPoint(),
                                             normal);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);

    MatrixRelease(matrix);
}

TEST(IntersectionTest, IntersectionCreateInfiniteTy)
{
    PMATRIX matrix;
    ISTATUS status =  MatrixAllocateScalar((float_t)2.0,
                                           (float_t)2.0,
                                           (float_t)-2.0,
                                           &matrix);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    VECTOR3 normal = VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0);
    INTERSECTION actual = IntersectionCreate(CreateWorldRay(),
                                             matrix,
                                             CreateModelHitPoint(),
                                             CreateWorldHitPoint(),
                                             normal);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);

    MatrixRelease(matrix);
}