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

#include "googletest/include/gtest/gtest.h"
#include "test/test_util.h"

#include <limits>

TEST(ColorTest, ColorTestCreate)
{
    COLOR3 actual = ColorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);

    COLOR3 expected;
    expected.red = (float_t)1.0;
    expected.green = (float_t)2.0;
    expected.blue = (float_t)3.0;

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

TEST(ColorTest, ColorAverageComponents)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 2.0);
    float_t actual = ColorAverageComponents(c0);
    EXPECT_EQ((float_t)2.0, actual);
}

TEST(ColorTest, ColorValidate)
{
    COLOR3 color = ColorCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    EXPECT_TRUE(ColorValidate(color));

    color.red = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.red = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.red = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.red = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.red = (float_t) 0.0;

    color.green = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.green = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.green = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.green = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.green = (float_t) 0.0;
    
    color.blue = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.blue = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.blue = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.blue = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
}

TEST(ColorTest, ColorCreateBlack)
{
    COLOR3 actual = ColorCreateBlack();

    COLOR3 expected = ColorCreate((float_t) 0.0, 
                                  (float_t) 0.0, 
                                  (float_t) 0.0);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorIsBlack)
{
    COLOR3 c0 = ColorCreate((float_t) 1.0, (float_t) 3.0, (float_t) 1.0);
    EXPECT_FALSE(ColorIsBlack(c0));

    COLOR3 c1 = ColorCreateBlack();
    EXPECT_TRUE(ColorIsBlack(c1));
}