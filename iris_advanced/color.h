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
    COLOR_SPACE_XYZ         = 0,
    COLOR_SPACE_LINEAR_SRGB = 1,
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
ColorClamp(
    _In_ COLOR3 color,
    _In_ float max
    )
{
    assert(0.0f <= max);

    color.values[0] = fminf(color.values[0], max);
    color.values[1] = fminf(color.values[1], max);
    color.values[2] = fminf(color.values[2], max);

    return color;
}

COLOR3
ColorToXyz(
    _In_ COLOR3 color
    );

COLOR3
ColorConvert(
    _In_ COLOR3 color,
    _In_ COLOR_SPACE target
    );

COLOR3
ColorAdd(
    _In_ COLOR3 addend0,
    _In_ COLOR3 addend1,
    _In_ COLOR_SPACE sum_color_space
    );

#endif // _IRIS_ADVANCED_COLOR_