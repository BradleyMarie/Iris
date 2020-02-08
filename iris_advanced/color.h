/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color.h

Abstract:

    A color represented as a tuple of 3 floats and a color space.

--*/

#ifndef _IRIS_ADVANCED_COLOR_
#define _IRIS_ADVANCED_COLOR_

#include "iris/iris.h"

//
// Types
//

typedef enum _COLOR_SPACE {
    COLOR_SPACE_XYZ,
    COLOR_SPACE_LINEAR_SRGB
} COLOR_SPACE, *PCOLOR_SPACE;

typedef const COLOR_SPACE *PCCOLOR_SPACE;

typedef struct _COLOR3 {
    float values[3];
    COLOR_SPACE color_space;
} COLOR3, *PCOLOR3;

typedef const COLOR3 *PCCOLOR3;

//
// Functions
//

static
inline
bool
ColorValidate(
    _In_ COLOR3 color
    )
{
    if (!isfinite(color.values[0]) || color.values[0] < (float_t)0.0 ||
        !isfinite(color.values[1]) || color.values[1] < (float_t)0.0 ||
        !isfinite(color.values[2]) || color.values[2] < (float_t)0.0 ||
        (color.color_space != COLOR_SPACE_XYZ &&
         color.color_space != COLOR_SPACE_LINEAR_SRGB))
    {
        return false;
    }

    return true;
}

static
inline
COLOR3
ColorCreate(
    _In_ COLOR_SPACE color_space,
    _In_ const float_t values[3]
    )
{
    COLOR3 color;
    color.values[0] = (float)values[0];
    color.values[1] = (float)values[1];
    color.values[2] = (float)values[2];
    color.color_space = color_space;
    assert(ColorValidate(color));

    return color;
}

static
inline
COLOR3
ColorCreateBlack(
    void
    )
{
    const float_t values[3] = { (float_t)0.0, (float_t)0.0, (float_t)0.0 };
    return ColorCreate(COLOR_SPACE_XYZ, values);
}

static
inline
COLOR3
ColorScale(
    _In_ COLOR3 color,
    _In_ float scalar
    )
{
    assert(isfinite(scalar));
    assert((float_t)0.0 <= scalar);

    COLOR3 product;
    product.values[0] = color.values[0] * scalar;
    product.values[1] = color.values[1] * scalar;
    product.values[2] = color.values[2] * scalar;
    product.color_space = color.color_space;
    assert(ColorValidate(product));

    return product;
}

static
inline
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
    const float conversion_matrices[][3][3] = {
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

static
inline
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
    const float conversion_matrices[][3][3] = {
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
    result.values[0] = fmaxf(0.0f, result.values[0]);

    result.values[1] = color.values[0] * conversion_matrices[index][1][0] +
                       color.values[1] * conversion_matrices[index][1][1] +
                       color.values[2] * conversion_matrices[index][1][2];
    result.values[1] = fmaxf(0.0f, result.values[1]);

    result.values[2] = color.values[0] * conversion_matrices[index][2][0] +
                       color.values[1] * conversion_matrices[index][2][1] +
                       color.values[2] * conversion_matrices[index][2][2];
    result.values[2] = fmaxf(0.0f, result.values[2]);

    result.color_space = target;
    assert(ColorValidate(result));

    return result;
}

static
inline
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

#endif // _IRIS_ADVANCED_COLOR_