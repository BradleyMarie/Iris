/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    point_test.cc

Abstract:

    Unit tests for point.h

--*/

extern "C" {
#include "iris/point.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(PointTest, PointCreate)
{
    POINT3 actual = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);

    POINT3 expected;
    expected.x = (float_t) 1.0;
    expected.y = (float_t) 2.0;
    expected.z = (float_t) 3.0;

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointSubtract)
{
    POINT3 origin = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    POINT3 dest = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    VECTOR3 actual = PointSubtract(dest, origin);
    
    VECTOR3 expected = VectorCreate((float_t) 1.0, 
                                    (float_t) 1.0, 
                                    (float_t) 1.0);

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointVectorSubtract)
{
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    VECTOR3 dir = VectorCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    POINT3 actual = PointVectorSubtract(origin, dir);

    POINT3 expected = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointVectorAdd)
{
    POINT3 origin = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    VECTOR3 dir = VectorCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);
    POINT3 actual = PointVectorAdd(origin, dir);

    POINT3 expected = PointCreate((float_t) 1.0, (float_t) 1.0, (float_t) 1.0);

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointVectorAddScaled)
{
    POINT3 origin = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    VECTOR3 dir = VectorCreate((float_t) 4.0, (float_t) 4.0, (float_t) 4.0);
    POINT3 actual = PointVectorAddScaled(origin, dir, (float_t) 0.5);

    POINT3 expected = PointCreate((float_t) 2.0, (float_t) 2.0, (float_t) 2.0);

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointVectorSubtractScaled)
{
    POINT3 origin = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    VECTOR3 dir = VectorCreate((float_t) -4.0, (float_t) -4.0, (float_t) -4.0);
    POINT3 actual = PointVectorSubtractScaled(origin, dir, (float_t) 0.5);

    POINT3 expected = PointCreate((float_t) 2.0, (float_t) 2.0, (float_t) 2.0);

    EXPECT_EQ(expected, actual);
}

TEST(PointTest, PointValidate)
{
    POINT3 point = PointCreate((float_t) 0.0, (float_t) 0.0, (float_t) 0.0);
    EXPECT_TRUE(PointValidate(point));

    point.x = (float_t) INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.x = (float_t) -INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.x = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(PointValidate(point));
    point.x = (float_t) 0.0;

    point.y = (float_t) INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.y = (float_t) -INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.y = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(PointValidate(point));
    point.y = (float_t) 0.0;
    
    point.z = (float_t) INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.z = (float_t) -INFINITY;
    EXPECT_FALSE(PointValidate(point));

    point.z = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(PointValidate(point));
    point.z = (float_t) 0.0;
}