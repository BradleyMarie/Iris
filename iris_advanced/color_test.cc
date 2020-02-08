/*++

Copyright (c) 2020 Brad Weinberger

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
#include "test_util/equality.h"

TEST(ColorTest, ColorValidate)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 color = ColorCreate(COLOR_SPACE_XYZ, values);
    EXPECT_TRUE(ColorValidate(color));

    color.values[0] = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[0] = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[0] = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.values[0] = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.values[0] = (float_t) 0.0;

    color.values[1] = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[1] = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[1] = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.values[1] = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.values[1] = (float_t) 0.0;

    color.values[2] = (float_t) INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[2] = (float_t) -INFINITY;
    EXPECT_FALSE(ColorValidate(color));

    color.values[2] = (float_t) -1.0;
    EXPECT_FALSE(ColorValidate(color));

    color.values[2] = std::numeric_limits<float_t>::quiet_NaN();
    EXPECT_FALSE(ColorValidate(color));
    color.values[2] = (float_t) 0.0;

    color.color_space = (COLOR_SPACE)(COLOR_SPACE_XYZ - 1);
    EXPECT_FALSE(ColorValidate(color));

    color.color_space = (COLOR_SPACE)(COLOR_SPACE_LINEAR_SRGB + 1);
    EXPECT_FALSE(ColorValidate(color));
}

TEST(ColorTest, ColorTestCreate)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 actual = ColorCreate(COLOR_SPACE_XYZ, values);

    COLOR3 expected;
    expected.values[0] = (float_t)1.0;
    expected.values[1] = (float_t)2.0;
    expected.values[2] = (float_t)3.0;
    expected.color_space = COLOR_SPACE_XYZ;

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorTestCreateBlack)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 actual = ColorCreateBlack();

    COLOR3 expected;
    expected.values[0] = (float_t)0.0;
    expected.values[1] = (float_t)0.0;
    expected.values[2] = (float_t)0.0;
    expected.color_space = COLOR_SPACE_XYZ;

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorScaleByScalar)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 color = ColorCreate(COLOR_SPACE_XYZ, values);
    COLOR3 actual = ColorScale(color, 2.0f);

    float expected_values[3] = {2.0f, 4.0f, 6.0f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_XYZ, expected_values);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorToXyzXyz)
{
    float xyz_values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 expected_xyz_color = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    COLOR3 actual_xyz_color = ColorToXyz(expected_xyz_color);
    EXPECT_EQ(expected_xyz_color, actual_xyz_color);
}

TEST(ColorTest, ColorToXyzLinearSrgb)
{
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 srgb_color = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 srgb_actual_xyz_color = ColorToXyz(srgb_color);

    float srgb_expected_xyz_values[3] =
        {0.475235015f, 0.50000006f, 0.544414997f};
    COLOR3 srgb_expected_xyz_color = ColorCreate(COLOR_SPACE_XYZ,
                                                 srgb_expected_xyz_values);
    EXPECT_THAT(srgb_expected_xyz_color,
                ApproximatelyEqualsColor(srgb_actual_xyz_color, 0.01f));
}

TEST(ColorTest, ColorConvertXyzToXyz)
{
    float xyz_values[3] = {0.5f, 2.0f, 3.0f};
    COLOR3 xyz_color = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    EXPECT_EQ(xyz_color, ColorConvert(xyz_color, COLOR_SPACE_XYZ));
}

TEST(ColorTest, ColorConvertLinearSrgbToLinearSrgb)
{
    float xyz_values[3] = {0.5f, 2.0f, 3.0f};
    COLOR3 xyz_color = ColorCreate(COLOR_SPACE_LINEAR_SRGB, xyz_values);
    EXPECT_EQ(xyz_color, ColorConvert(xyz_color, COLOR_SPACE_LINEAR_SRGB));
}

TEST(ColorTest, ColorConvertLinearSrgbToXyz)
{
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 srgb_color = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 srgb_actual_xyz_color = ColorConvert(srgb_color, COLOR_SPACE_XYZ);

    float srgb_expected_xyz_values[3] =
        {0.475235015f, 0.50000006f, 0.544414997f};
    COLOR3 srgb_expected_xyz_color = ColorCreate(COLOR_SPACE_XYZ,
                                                 srgb_expected_xyz_values);
    EXPECT_THAT(srgb_expected_xyz_color,
                ApproximatelyEqualsColor(srgb_actual_xyz_color, 0.01f));
}

TEST(ColorTest, ColorConvertXyzToLinearSrgb)
{
    float xyz_values[3] =
        {0.475235015f, 0.50000006f, 0.544414997f};
    COLOR3 xyz_color = ColorCreate(COLOR_SPACE_XYZ, xyz_values);

    float expected_srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 expected_srgb_color =
        ColorCreate(COLOR_SPACE_LINEAR_SRGB, expected_srgb_values);
    COLOR3 actual_srgb_color = ColorConvert(xyz_color, COLOR_SPACE_LINEAR_SRGB);
    EXPECT_THAT(expected_srgb_color,
                ApproximatelyEqualsColor(actual_srgb_color, 0.01f));
}

TEST(ColorTest, ColorAddXyzToXyzToXyz)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_XYZ, values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_XYZ, values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_XYZ);

    float expected_values[3] = {2.0f, 4.0f, 6.0f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_XYZ, expected_values);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorAddLinearSrgbToLinearSrgbToLinearSrgb)
{
    float values[3] = {1.0f, 2.0f, 3.0f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_LINEAR_SRGB);

    float expected_values[3] = {2.0f, 4.0f, 6.0f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_LINEAR_SRGB, expected_values);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorAddLinearSrgbToLinearSrgbToXyz)
{
    float values[3] = {0.25f, 0.25f, 0.25f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_XYZ);

    float expected_values[3] =
        {0.475235015f, 0.50000006f, 0.544414997f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_XYZ, expected_values);

    EXPECT_EQ(expected, actual);
}

TEST(ColorTest, ColorAddLinearSrgbToXyzToXyz)
{
    float xyz_values[3] = {0.5f, 0.5f, 0.5f};
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_XYZ);

    float expected_values[3] =
        {0.975235015f, 1.00000006f, 1.044414997f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_XYZ, expected_values);

    EXPECT_THAT(expected, ApproximatelyEqualsColor(actual, 0.01f));
}

TEST(ColorTest, ColorAddXyzToLinearSrgbToXyz)
{
    float xyz_values[3] = {0.5f, 0.5f, 0.5f};
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_XYZ);

    float expected_values[3] =
        {0.975235015f, 1.00000006f, 1.044414997f};
    COLOR3 expected = ColorCreate(COLOR_SPACE_XYZ, expected_values);

    EXPECT_THAT(expected, ApproximatelyEqualsColor(actual, 0.01f));
}

TEST(ColorTest, ColorAddLinearSrgbToXyzToLinearSrgb)
{
    float xyz_values[3] = {0.475235015f, 0.50000006f, 0.544414997f};
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_LINEAR_SRGB);

    float expected_values[3] = { 1.0f, 1.0f, 1.0f };
    COLOR3 expected = ColorCreate(COLOR_SPACE_LINEAR_SRGB, expected_values);

    EXPECT_THAT(expected, ApproximatelyEqualsColor(actual, 0.01f));
}

TEST(ColorTest, ColorAddXyzToLinearSrgbToLinearSrgb)
{
    float xyz_values[3] = {0.475235015f, 0.50000006f, 0.544414997f};
    float srgb_values[3] = {0.5f, 0.5f, 0.5f};
    COLOR3 c0 = ColorCreate(COLOR_SPACE_LINEAR_SRGB, srgb_values);
    COLOR3 c1 = ColorCreate(COLOR_SPACE_XYZ, xyz_values);
    COLOR3 actual = ColorAdd(c0, c1, COLOR_SPACE_LINEAR_SRGB);

    float expected_values[3] = { 1.0f, 1.0f, 1.0f };
    COLOR3 expected = ColorCreate(COLOR_SPACE_LINEAR_SRGB, expected_values);

    EXPECT_THAT(expected, ApproximatelyEqualsColor(actual, 0.01f));
}