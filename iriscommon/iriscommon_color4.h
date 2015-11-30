/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_color4.h

Abstract:

    This file contains the definitions for the COLOR4 type.

--*/

#ifndef _IRIS_COMMON_COLOR4_
#define _IRIS_COMMON_COLOR4_

#include <irisadvanced.h>

//
// Types
//

typedef struct _COLOR4 {
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;
} COLOR4, *PCOLOR4;

typedef CONST COLOR4 *PCCOLOR4;

//
// Functions
//

SFORCEINLINE
COLOR4
Color4InitializeFromComponents(
    _In_ FLOAT Red,
    _In_ FLOAT Green,
    _In_ FLOAT Blue,
    _In_ FLOAT Alpha
    )
{
    COLOR4 Color;

    ASSERT(IsFiniteFloat(Red) != FALSE);
    ASSERT(IsFiniteFloat(Green) != FALSE);
    ASSERT(IsFiniteFloat(Blue) != FALSE);
    ASSERT(IsFiniteFloat(Alpha) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Alpha) != FALSE);
    ASSERT(Alpha <= (FLOAT) 1.0);

    Color.Red = Red;
    Color.Green = Green;
    Color.Blue = Blue;
    Color.Alpha = Alpha;

    return Color;
}

SFORCEINLINE
COLOR4
Color4InitializeFromColor3(
    _In_ COLOR3 Color3,
    _In_ FLOAT Alpha
    )
{
    COLOR4 Color;

    ASSERT(IsGreaterThanOrEqualToZeroFloat(Alpha) != FALSE);
    ASSERT(Alpha <= (FLOAT) 1.0);

    Color = Color4InitializeFromComponents(Color3.Red,
                                           Color3.Green,
                                           Color3.Blue,
                                           Alpha);

    return Color;
}

SFORCEINLINE
COLOR3
Color3InitializeFromColor4(
    _In_ COLOR4 Color4
    )
{
    COLOR3 Color;

    Color = Color3InitializeFromComponents(Color4.Red,
                                           Color4.Green,
                                           Color4.Blue);

    return Color;
}

SFORCEINLINE
COLOR4
Color4Add(
    _In_ COLOR4 Addend0,
    _In_ COLOR4 Addend1
    )
{
    COLOR4 Sum;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    Red = Addend0.Red + Addend1.Red;
    Green = Addend0.Green + Addend1.Green;
    Blue = Addend0.Blue + Addend1.Blue;
    Alpha = Addend0.Alpha + Addend1.Alpha;

    Sum = Color4InitializeFromComponents(Red, Green, Blue, Alpha);

    return Sum;
}

SFORCEINLINE
COLOR4
Color4ScaleByColor(
    _In_ COLOR4 Color,
    _In_ COLOR4 Scalar
    )
{
    COLOR4 Scaled;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    Red = Color.Red * Scalar.Red;
    Green = Color.Green * Scalar.Green;
    Blue = Color.Blue * Scalar.Blue;
    Alpha = Color.Alpha * Scalar.Alpha;

    Scaled = Color4InitializeFromComponents(Red, Green, Blue, Alpha);

    return Scaled;
}

SFORCEINLINE
COLOR4
Color4ScaleByScalar(
    _In_ COLOR4 Color,
    _In_ FLOAT Scalar
    )
{
    COLOR4 Scaled;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    Red = Color.Red * Scalar;
    Green = Color.Green * Scalar;
    Blue = Color.Blue * Scalar;
    Alpha = Color.Alpha * Scalar;

    Scaled = Color4InitializeFromComponents(Red, Green, Blue, Alpha);

    return Scaled;
}

SFORCEINLINE
COLOR4
Color4Over(
    _In_ COLOR4 Over,
    _In_ COLOR4 Under
    )
{
    COLOR4 Blended;
    FLOAT AlphaScalar;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    AlphaScalar = Under.Alpha * ((FLOAT) 1.0 - Over.Alpha);

    Red = Over.Red * Over.Alpha + Under.Red * AlphaScalar;
    Green = Over.Green * Over.Alpha + Under.Green * AlphaScalar;
    Blue = Over.Blue * Over.Alpha + Under.Blue * AlphaScalar;
    Alpha = MinFloat(Over.Alpha + AlphaScalar, (FLOAT) 1.0);

    Blended = Color4InitializeFromComponents(Red, Green, Blue, Alpha);

    return Blended;
}

SFORCEINLINE
COLOR4
Color4InitializeTransparent(
    VOID
    )
{
    COLOR4 Color;

    Color = Color4InitializeFromComponents((FLOAT) 0.0,
                                           (FLOAT) 0.0,
                                           (FLOAT) 0.0,
                                           (FLOAT) 0.0);

    return Color;
}

#endif // _IRIS_COMMON_COLOR4_