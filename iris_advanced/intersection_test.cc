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

RAY_DIFFERENTIAL
CreateModelRay(
    void
    )
{
    POINT3 origin = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    VECTOR3 direction = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-2.0);
    VECTOR3 rx = VectorCreate((float_t)1.0, (float_t)0.0, (float_t)-1.0);
    rx = VectorNormalize(rx, NULL, NULL);
    rx = VectorScale(rx, (float_t)2.0);
    VECTOR3 ry = VectorCreate((float_t)0.0, (float_t)1.0, (float_t)-1.0);
    ry = VectorNormalize(ry, NULL, NULL);
    ry = VectorScale(ry, (float_t)2.0);

    RAY ray0 = RayCreate(origin, direction);
    RAY ray1 = RayCreate(origin, rx);
    RAY ray2 = RayCreate(origin, ry);

    return RayDifferentialCreate(ray0, ray1, ray2);
}

TEST(IntersectionTest, IntersectionCreateModel)
{
    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    INTERSECTION actual = IntersectionCreate(CreateModelRay(),
                                             CreateWorldRay(),
                                             normal,
                                             NORMAL_MODEL_COORDINATE_SPACE,
                                             (float_t)1.0);

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
}

TEST(IntersectionTest, IntersectionCreateWorld)
{
    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0);
    INTERSECTION actual = IntersectionCreate(CreateModelRay(),
                                             CreateWorldRay(),
                                             normal,
                                             NORMAL_WORLD_COORDINATE_SPACE,
                                             (float_t)1.0);

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
}

TEST(IntersectionTest, IntersectionCreateNoDifferentials)
{
    RAY_DIFFERENTIAL model_ray = CreateModelRay();
    model_ray.has_differentials = false;
    RAY_DIFFERENTIAL world_ray = CreateWorldRay();
    world_ray.has_differentials = false;

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0);
    INTERSECTION actual = IntersectionCreate(model_ray,
                                             world_ray,
                                             normal,
                                             NORMAL_WORLD_COORDINATE_SPACE,
                                             (float_t)1.0);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);
}

TEST(IntersectionTest, IntersectionCreateInfiniteTx)
{
    RAY_DIFFERENTIAL model_ray = CreateModelRay();
    model_ray.rx.direction.y = (float)1.0;
    model_ray.rx.direction.z = (float)0.0;

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    INTERSECTION actual = IntersectionCreate(model_ray,
                                             CreateWorldRay(),
                                             normal,
                                             NORMAL_MODEL_COORDINATE_SPACE,
                                             (float_t)1.0);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);
}

TEST(IntersectionTest, IntersectionCreateInfiniteTy)
{
    RAY_DIFFERENTIAL model_ray = CreateModelRay();
    model_ray.ry.direction.y = (float)1.0;
    model_ray.ry.direction.z = (float)0.0;

    VECTOR3 normal = VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0);
    INTERSECTION actual = IntersectionCreate(model_ray,
                                             CreateWorldRay(),
                                             normal,
                                             NORMAL_MODEL_COORDINATE_SPACE,
                                             (float_t)1.0);

    EXPECT_NEAR(actual.model_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.model_hit_point.z, (float_t)-2.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.x, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.y, (float_t)0.0, (float_t)0.001);
    EXPECT_NEAR(actual.world_hit_point.z, (float_t)1.0, (float_t)0.001);
    EXPECT_FALSE(actual.has_derivatives);
}