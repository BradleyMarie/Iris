/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color.h

Abstract:

    A color represented as an XYZ tuple.

--*/

#ifndef _IRIS_ADVANCED_COLOR_
#define _IRIS_ADVANCED_COLOR_

#include "iris/iris.h"

//
// Types
//

typedef struct _COLOR3 {
    float_t x;
    float_t y;
    float_t z;
} COLOR3, *PCOLOR3;

typedef const COLOR3 *PCCOLOR3;

//
// Functions
//

static
inline
COLOR3
ColorCreate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z
    )
{
    assert(isfinite(x));
    assert((float_t)0.0 <= x);
    assert(isfinite(y));
    assert((float_t)0.0 <= y);
    assert(isfinite(z));
    assert((float_t)0.0 <= z);

    COLOR3 color;
    color.x = x;
    color.y = y;
    color.z = z;

    return color;
}

static
inline
bool
ColorValidate(
    _In_ COLOR3 color
    )
{
    if (!isfinite(color.x) || color.x < (float_t)0.0 ||
        !isfinite(color.y) || color.y < (float_t)0.0 ||
        !isfinite(color.z) || color.z < (float_t)0.0)
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
    float_t x = addend0.x + addend1.x;
    float_t y = addend0.y + addend1.y;
    float_t z = addend0.z + addend1.z;

    return ColorCreate(x, y, z);
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

    float_t x = fma(scalar, addend1.x, addend0.x);
    float_t y = fma(scalar, addend1.y, addend0.y);
    float_t z = fma(scalar, addend1.z, addend0.z);

    return ColorCreate(x, y, z);
}

static
inline
COLOR3
ColorScaleByColor(
    _In_ COLOR3 color,
    _In_ COLOR3 scalar
    )
{
    float_t x = color.x * scalar.x;
    float_t y = color.y * scalar.y;
    float_t z = color.z * scalar.z;

    return ColorCreate(x, y, z);
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

    float_t x = color.x * scalar;
    float_t y = color.y * scalar;
    float_t z = color.z * scalar;

    return ColorCreate(x, y, z);
}

#endif // _IRIS_ADVANCED_COLOR_