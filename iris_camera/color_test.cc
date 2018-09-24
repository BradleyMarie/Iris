/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_test.cc

Abstract:

    Unit tests for color.h

--*/

extern "C" {
#include "iris_advanced/color.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"
#include "test/test_util.h"

TEST(ColorTest, ColorTestCreate)
{
    COLOR3 actual = ColorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);

    COLOR3 expected;
    expected.x = (float_t)1.0;
    expected.y = (float_t)2.0;
    expected.z = (float_t)3.0;

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorAdd)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 1.0);
    COLOR3 c1 = ColorCreate((float_t) 2.0, (float_t) 2.0, (float_t) 1.0);
    COLOR3 actual = ColorAdd(c0, c1);

    COLOR3 expected = ColorCreate((float_t) 3.0, 
                                  (float_t) 5.0, 
                                  (float_t) 2.0);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorAddScaled)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 1.0);
    COLOR3 c1 = ColorCreate((float_t) 2.0, (float_t) 2.0, (float_t) 1.0);
    COLOR3 actual = ColorAddScaled(c0, c1, (float_t)2.0);

    COLOR3 expected = ColorCreate((float_t) 5.0, 
                                  (float_t) 7.0, 
                                  (float_t) 3.0);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorScaleByColor)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 1.0);
    COLOR3 c1 = ColorCreate((float_t) 2.0, (float_t) 2.0, (float_t) 1.0);
    COLOR3 actual = ColorScaleByColor(c0, c1);

    COLOR3 expected = ColorCreate((float_t) 2.0, 
                                  (float_t) 6.0, 
                                  (float_t) 1.0);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorScaleByScalar)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 1.0);
    COLOR3 actual = ColorScaleByScalar(c0, (float_t)2.0);

    COLOR3 expected = ColorCreate((float_t) 2.0, 
                                  (float_t) 6.0, 
                                  (float_t) 2.0);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorValidate)
{
    COLOR3 color = ColorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    EXPECT_TRUE(ColorValidate(color));

    color.x = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.x = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.x = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.x = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.x = (float_t) 0.0;

    color.y = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.y = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.y = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.y = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.y = (float_t) 0.0;
    
    color.z = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.z = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.z = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.z = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
}