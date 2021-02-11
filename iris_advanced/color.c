/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color.c

Abstract:

    A color represented as a tuple of 3 floats and a color space.

--*/

#include "iris_advanced/color.h"

//
// Functions
//

bool
ColorIsBlack(
    _In_ COLOR3 color
    )
{
    bool result = color.values[0] == (float_t)0.0 &&
                  color.values[1] == (float_t)0.0 &&
                  color.values[2] == (float_t)0.0;

    return result;
}

COLOR3
ColorToXyz(
    _In_ COLOR3 color
    )
{
    if (color.color_space == COLOR_SPACE_XYZ)
    {
        return color;
    }

    size_t index = (size_t)color.color_space;
    static const float conversion_matrices[2][3][3] = {
        // COLOR_SPACE_XYZ
        { { 1.0000000f, 1.0000000f, 1.0000000f },
          { 1.0000000f, 1.0000000f, 1.0000000f },
          { 1.0000000f, 1.0000000f, 1.0000000f } },
        // COLOR_SPACE_LINEAR_SRGB
        { { 0.4124564f, 0.3575761f, 0.1804375f },
          { 0.2126729f, 0.7151522f, 0.0721750f },
          { 0.0193339f, 0.1191920f, 0.9503041f } },
    };

    COLOR3 result;
    result.values[0] = color.values[0] * conversion_matrices[index][0][0] +
                       color.values[1] * conversion_matrices[index][0][1] +
                       color.values[2] * conversion_matrices[index][0][2];
    result.values[1] = color.values[0] * conversion_matrices[index][1][0] +
                       color.values[1] * conversion_matrices[index][1][1] +
                       color.values[2] * conversion_matrices[index][1][2];
    result.values[2] = color.values[0] * conversion_matrices[index][2][0] +
                       color.values[1] * conversion_matrices[index][2][1] +
                       color.values[2] * conversion_matrices[index][2][2];
    result.color_space = COLOR_SPACE_XYZ;
    assert(ColorValidate(result));

    return result;
}

COLOR3
ColorConvert(
    _In_ COLOR3 color,
    _In_ COLOR_SPACE target
    )
{
    if (color.color_space == target)
    {
        return color;
    }

    COLOR3 xyz = ColorToXyz(color);

    if (target == COLOR_SPACE_XYZ)
    {
        return xyz;
    }

    size_t index = (size_t)target;
    static const float conversion_matrices[2][3][3] = {
        // COLOR_SPACE_XYZ
        { {  1.0000000f,  1.0000000f,  1.0000000f },
          {  1.0000000f,  1.0000000f,  1.0000000f },
          {  1.0000000f,  1.0000000f,  1.0000000f } },
        // COLOR_SPACE_LINEAR_SRGB
        { {  3.2404542f, -1.5371385f, -0.4985314f },
          { -0.9692660f,  1.8760108f,  0.0415560f },
          {  0.0556434f, -0.2040259f,  1.0572252f } },
    };

    COLOR3 result;
    result.values[0] = color.values[0] * conversion_matrices[index][0][0] +
                       color.values[1] * conversion_matrices[index][0][1] +
                       color.values[2] * conversion_matrices[index][0][2];
    result.values[0] = IMax(0.0f, result.values[0]); // was fmaxf

    result.values[1] = color.values[0] * conversion_matrices[index][1][0] +
                       color.values[1] * conversion_matrices[index][1][1] +
                       color.values[2] * conversion_matrices[index][1][2];
    result.values[1] = IMax(0.0f, result.values[1]); // was fmaxf

    result.values[2] = color.values[0] * conversion_matrices[index][2][0] +
                       color.values[1] * conversion_matrices[index][2][1] +
                       color.values[2] * conversion_matrices[index][2][2];
    result.values[2] = IMax(0.0f, result.values[2]); // was fmaxf

    result.color_space = target;
    assert(ColorValidate(result));

    return result;
}

COLOR3
ColorAdd(
    _In_ COLOR3 addend0,
    _In_ COLOR3 addend1,
    _In_ COLOR_SPACE sum_color_space
    )
{
    if (addend0.color_space == addend1.color_space)
    {
        COLOR3 sum;
        sum.values[0] = addend0.values[0] + addend1.values[0];
        sum.values[1] = addend0.values[1] + addend1.values[1];
        sum.values[2] = addend0.values[2] + addend1.values[2];
        sum.color_space = addend0.color_space;
        return ColorConvert(sum, sum_color_space);
    }

    COLOR_SPACE intermediate_color_space;
    if (addend0.color_space == sum_color_space)
    {
        addend1 = ColorConvert(addend1, sum_color_space);
        intermediate_color_space = sum_color_space;
    }
    else if (addend1.color_space == sum_color_space)
    {
        addend0 = ColorConvert(addend0, sum_color_space);
        intermediate_color_space = sum_color_space;
    }
    else
    {
        addend0 = ColorToXyz(addend0);
        addend1 = ColorToXyz(addend1);
        intermediate_color_space = COLOR_SPACE_XYZ;
    }

    COLOR3 sum;
    sum.values[0] = addend0.values[0] + addend1.values[0];
    sum.values[1] = addend0.values[1] + addend1.values[1];
    sum.values[2] = addend0.values[2] + addend1.values[2];
    sum.color_space = intermediate_color_space;

    return ColorConvert(sum, sum_color_space);
}