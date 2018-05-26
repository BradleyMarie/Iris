/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray_test.cc

Abstract:

    Unit tests for ray.h

--*/

extern "C" {
#include "iris/ray.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test/test_util.h"

TEST(RayTest, RayCreate)
{
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0, 
                                     (float_t) 6.0);
    RAY actual = RayCreate(origin, direction);

    RAY expected;
    expected.origin = origin;
    expected.direction = direction;

    EXPECT_EQ(expected, actual);
}

TEST(RayTest, RayEndpoint)
{
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0, 
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);
    POINT3 actual = RayEndpoint(ray, (float_t) 2.0);
    
    POINT3 expected = PointCreate((float_t) 9.0, 
                                  (float_t) 12.0, 
                                  (float_t) 15.0);

    EXPECT_EQ(expected, actual);
}

TEST(RayTest, RayNormalize)
{
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0, 
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);
    RAY actual = RayNormalize(ray);
    
    EXPECT_EQ(origin, actual.origin);
    EXPECT_NEAR((float_t) 0.455842, actual.direction.x, (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.569802, actual.direction.y, (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.683763, actual.direction.z, (float_t) 0.0001);
}

TEST(VectorTest, RayValidate)
{
    POINT3 origin = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    VECTOR3 direction = VectorCreate((float_t) 0.0, 
                                     (float_t) 0.0, 
                                     (float_t) 1.0);
    RAY ray = RayCreate(origin, direction);
    EXPECT_TRUE(RayValidate(ray));

    ray.origin.x = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
    ray.origin.x = (float_t) 0.0;

    ray.origin.y = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
    ray.origin.y = (float_t) 0.0;
    
    ray.origin.z = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
    ray.origin.z = (float_t) 0.0;

    ray.direction.x = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
    ray.direction.x = (float_t) 0.0;

    ray.direction.y = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
    ray.direction.y = (float_t) 0.0;
    
    ray.direction.z = (float_t) INFINITY;
    EXPECT_FALSE(RayValidate(ray));
}