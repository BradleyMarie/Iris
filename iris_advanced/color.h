/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    color.h

Abstract:

    A color represented as an RGB tuple.

--*/

#ifndef _IRIS_ADVANCED_COLOR_
#define _IRIS_ADVANCED_COLOR_

#include "iris/iris.h"

//
// Types
//

typedef struct _COLOR3 {
    float_t red;
    float_t green;
    float_t blue;
} COLOR3, *PCOLOR3;

typedef const COLOR3 *PCCOLOR3;

//
// Functions
//

static
inline
COLOR3
ColorCreate(
    _In_ float_t red,
    _In_ float_t green,
    _In_ float_t blue
    )
{
    assert(isfinite(red));
    assert((float_t)0.0 <= red);
    assert(isfinite(green));
    assert((float_t)0.0 <= green);
    assert(isfinite(blue));
    assert((float_t)0.0 <= blue);

    COLOR3 color;
    color.red = red;
    color.green = green;
    color.blue = blue;

    return color;
}

static
inline
bool
ColorValidate(
    _In_ COLOR3 color
    )
{
    if (!isfinite(color.red) || color.red < (float_t)0.0 ||
        !isfinite(color.green) || color.green < (float_t)0.0 ||
        !isfinite(color.blue) || color.blue < (float_t)0.0)
    {
        return false;
    }

    return true;
}

static
inline
COLOR3
ColorAdd(
    _In_ COLOR3 addend0,
    _In_ COLOR3 addend1
    )
{
    float_t red = addend0.red + addend1.red;
    float_t green = addend0.green + addend1.green;
    float_t blue = addend0.blue + addend1.blue;

    return ColorCreate(red, green, blue);
}

static
inline
COLOR3
ColorAddScaled(
    _In_ COLOR3 addend0,
    _In_ COLOR3 addend1,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar));
    assert((float_t)0.0 <= scalar);

    float_t red = fma(scalar, addend1.red, addend0.red);
    float_t green = fma(scalar, addend1.green, addend0.green);
    float_t blue = fma(scalar, addend1.blue, addend0.blue);

    return ColorCreate(red, green, blue);
}

static
inline
COLOR3
ColorScaleByColor(
    _In_ COLOR3 color,
    _In_ COLOR3 scalar
    )
{
    float_t red = color.red * scalar.red;
    float_t green = color.green * scalar.green;
    float_t blue = color.blue * scalar.blue;

    return ColorCreate(red, green, blue);
}

static
inline
COLOR3
ColorScaleByScalar(
    _In_ COLOR3 color,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar));
    assert((float_t)0.0 <= scalar);

    float_t red = color.red * scalar;
    float_t green = color.green * scalar;
    float_t blue = color.blue * scalar;

    return ColorCreate(red, green, blue);
}

static
inline
float_t
ColorAverageComponents(
    _In_ COLOR3 color
    )
{
    return (color.red + color.green + color.blue) / (float_t)3.0;
}

static
inline
COLOR3
ColorCreateBlack(
    void
    )
{
    return ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
}

static
inline
bool
ColorIsBlack(
    _In_ COLOR3 color
    )
{
    if (color.red == (float_t)0.0 &&
        color.green == (float_t)0.0 &&
        color.blue == (float_t)0.0)
    {
        return true;
    }

    return false;
}

#endif // _IRIS_ADVANCED_COLOR_