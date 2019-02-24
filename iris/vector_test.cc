/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    vector_test.cc

Abstract:

    Unit tests for vector.h

--*/

extern "C" {
#include "iris/vector.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(VectorTest, VectorCreate)
{
    VECTOR3 actual = VectorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);

    VECTOR3 expected;
    expected.x = (float_t) 1.0;
    expected.y = (float_t) 2.0;
    expected.z = (float_t) 3.0;

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorNegate)
{
    VECTOR3 v0 = VectorCreate((float_t) -1.0, (float_t) -2.0, (float_t) -3.0);
    VECTOR3 actual = VectorNegate(v0);

    VECTOR3 expected = VectorCreate((float_t) 1.0, 
                                    (float_t) 2.0, 
                                    (float_t) 3.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorAdd)
{
    VECTOR3 v0 = VectorCreate((float_t) 1.0, (float_t) 3.0, (float_t) -1.0);
    VECTOR3 v1 = VectorCreate((float_t) 2.0, (float_t) 2.0, (float_t)  1.0);
    VECTOR3 actual = VectorAdd(v0, v1);

    VECTOR3 expected = VectorCreate((float_t) 3.0, 
                                    (float_t) 5.0, 
                                    (float_t) 0.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorAddScaled)
{
    VECTOR3 v0 = VectorCreate((float_t) 1.0, (float_t) 3.0, (float_t) -1.0);
    VECTOR3 v1 = VectorCreate((float_t) 2.0, (float_t) 2.0, (float_t)  1.0);
    VECTOR3 actual = VectorAddScaled(v0, v1, (float_t) 2.0);

    VECTOR3 expected = VectorCreate((float_t) 5.0, 
                                    (float_t) 7.0, 
                                    (float_t) 1.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorSubtract)
{
    VECTOR3 v0 = VectorCreate((float_t) 1.0, (float_t) 3.0, (float_t)-1.0);
    VECTOR3 v1 = VectorCreate((float_t) 2.0, (float_t) 2.0, (float_t) 1.0);
    VECTOR3 actual = VectorSubtract(v0, v1);

    VECTOR3 expected = VectorCreate((float_t) -1.0,
                                    (float_t) 1.0, 
                                    (float_t) -2.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorScale)
{
    VECTOR3 v0 = VectorCreate((float_t) -1.0, (float_t) 1.0, (float_t) 1.0);
    VECTOR3 actual = VectorScale(v0, (float_t) 2.0);
    
    VECTOR3 expected = VectorCreate((float_t) -2.0, 
                                    (float_t) 2.0, 
                                    (float_t) 2.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorDotProduct)
{
    VECTOR3 v0 = VectorCreate((float_t) 0.0, (float_t) 1.0, (float_t) 2.0);
    VECTOR3 v1 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    float_t actual = VectorDotProduct(v0, v1);

    EXPECT_EQ((float_t) 7.0, actual);
}

TEST(VectorTest, VectorCrossProduct)
{
    VECTOR3 v0 = VectorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 v1 = VectorCreate((float_t) 3.0, (float_t) 2.0, (float_t) 1.0);
    VECTOR3 actual = VectorCrossProduct(v0, v1);

    VECTOR3 expected = VectorCreate((float_t) -4.0, 
                                    (float_t) 8.0, 
                                    (float_t)-4.0);

    EXPECT_EQ(expected, actual);
}

TEST(VectorTest, VectorLength)
{
    VECTOR3 v0 = VectorCreate((float_t) 0.0, (float_t) 0.0, (float_t) 1.0);
    EXPECT_EQ((float_t) 1.0, VectorLength(v0));

    VECTOR3 v1 = VectorCreate((float_t) 0.0, (float_t) 2.0, (float_t) 0.0);
    EXPECT_EQ((float_t) 2.0, VectorLength(v1));

    VECTOR3 v2 = VectorCreate((float_t) 4.0, (float_t) 0.0, (float_t) 0.0);
    EXPECT_EQ((float_t) 4.0, VectorLength(v2));
}

TEST(VectorTest, VectorValidate)
{
    VECTOR3 vector = VectorCreate((float_t) 0.0, (float_t) 0.0, (float_t) 1.0);
    EXPECT_TRUE(VectorValidate(vector));

    vector.x = (float_t) INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.x = (float_t) -INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.x = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(VectorValidate(vector));
    vector.x = (float_t) 0.0;

    vector.y = (float_t) INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.y = (float_t) -INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.y = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(VectorValidate(vector));
    vector.y = (float_t) 0.0;
    
    vector.z = (float_t) INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.z = (float_t) -INFINITY;
    EXPECT_FALSE(VectorValidate(vector));

    vector.z = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(VectorValidate(vector));
}

TEST(VectorTest, VectorDiminishedAxis)
{
    VECTOR3 v0 = VectorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    EXPECT_EQ(VECTOR_X_AXIS, VectorDiminishedAxis(v0));

    VECTOR3 v1 = VectorCreate((float_t) 2.0, (float_t) 1.0, (float_t) 3.0);
    EXPECT_EQ(VECTOR_Y_AXIS, VectorDiminishedAxis(v1));

    VECTOR3 v2 = VectorCreate((float_t) 3.0, (float_t) 2.0, (float_t) 1.0);
    EXPECT_EQ(VECTOR_Z_AXIS, VectorDiminishedAxis(v2));
}

TEST(VectorTest, VectorDominantAxis)
{
    VECTOR3 v0 = VectorCreate((float_t) 3.0, (float_t) 2.0, (float_t) 1.0);
    EXPECT_EQ(VECTOR_X_AXIS, VectorDominantAxis(v0));

    VECTOR3 v1 = VectorCreate((float_t) 2.0, (float_t) 3.0, (float_t) 1.0);
    EXPECT_EQ(VECTOR_Y_AXIS, VectorDominantAxis(v1));

    VECTOR3 v2 = VectorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    EXPECT_EQ(VECTOR_Z_AXIS, VectorDominantAxis(v2));
}

TEST(VectorTest, VectorNormalize)
{
    VECTOR3 v0 = VectorCreate((float_t) 3.0, (float_t) 1.0, (float_t) 2.0);
    
    float_t old_length_squared;
    float_t old_length;
    VECTOR3 actual = VectorNormalize(v0, &old_length_squared, &old_length);

    EXPECT_NEAR((float_t) 0.801784, actual.x, (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.267261, actual.y, (float_t) 0.0001);
    EXPECT_NEAR((float_t) 0.534522, actual.z, (float_t) 0.0001);
    EXPECT_EQ((float_t) 14.0, old_length_squared);
    EXPECT_NEAR((float_t) 3.741657, old_length, (float_t) 0.0001);
}

TEST(VectorTest, VectorBoundedDotProduct)
{
    VECTOR3 v0 = VectorCreate((float_t) 0.0, (float_t) 1.0, (float_t) 2.0);
    VECTOR3 v1 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    EXPECT_EQ((float_t) 7.0, VectorBoundedDotProduct(v0, v1));

    VECTOR3 v2 = VectorCreate((float_t) -4.0, (float_t) -3.0, (float_t) -2.0);
    EXPECT_EQ((float_t) 0.0, VectorBoundedDotProduct(v0, v2));
}

TEST(VectorTest, VectorPositiveDotProduct)
{
    VECTOR3 v0 = VectorCreate((float_t) 0.0, (float_t) 1.0, (float_t) 2.0);
    VECTOR3 v1 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    EXPECT_EQ((float_t) 7.0, VectorPositiveDotProduct(v0, v1, false));
    EXPECT_EQ((float_t) 0.0, VectorPositiveDotProduct(v0, v1, true));

    VECTOR3 v2 = VectorCreate((float_t) -4.0, (float_t) -3.0, (float_t) -2.0);
    EXPECT_EQ((float_t) 0.0, VectorPositiveDotProduct(v0, v2, false));
    EXPECT_EQ((float_t) 7.0, VectorPositiveDotProduct(v0, v2, true));
}