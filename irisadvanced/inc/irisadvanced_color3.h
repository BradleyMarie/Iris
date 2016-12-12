/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_color3.h

Abstract:

    This file contains the definitions for the COLOR3 type.

--*/

#ifndef _IRIS_ADVANCED_COLOR3_
#define _IRIS_ADVANCED_COLOR3_

#include <irisadvanced.h>

//
// Types
//

typedef struct _COLOR3 {
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
} COLOR3, *PCOLOR3;

typedef CONST COLOR3 *PCCOLOR3;

//
// Functions
//

SFORCEINLINE
COLOR3
Color3InitializeFromComponents(
    _In_ FLOAT Red,
    _In_ FLOAT Green,
    _In_ FLOAT Blue
    )
{
    COLOR3 Color;

    ASSERT(IsFiniteFloat(Red) != FALSE);
    ASSERT(IsFiniteFloat(Green) != FALSE);
    ASSERT(IsFiniteFloat(Blue) != FALSE);

    Color.Red = Red;
    Color.Green = Green;
    Color.Blue = Blue;

    return Color;
}

SFORCEINLINE
BOOL
Color3Validate(
    _In_ COLOR3 Color
    )
{
    if (IsFiniteFloat(Color.Red) == FALSE ||
        IsFiniteFloat(Color.Green) == FALSE ||
        IsFiniteFloat(Color.Blue) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

SFORCEINLINE
COLOR3
Color3Add(
    _In_ COLOR3 Addend0,
    _In_ COLOR3 Addend1
    )
{
    COLOR3 Sum;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    Red = Addend0.Red + Addend1.Red;
    Green = Addend0.Green + Addend1.Green;
    Blue = Addend0.Blue + Addend1.Blue;

    Sum = Color3InitializeFromComponents(Red, Green, Blue);

    return Sum;
}

SFORCEINLINE
COLOR3
Color3AddScaled(
    _In_ COLOR3 Addend0,
    _In_ COLOR3 Addend1,
    _In_ FLOAT Scalar
    )
{
    COLOR3 Sum;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    Red = FmaFloat(Scalar, Addend1.Red, Addend0.Red);
    Green = FmaFloat(Scalar, Addend1.Green, Addend0.Green);
    Blue = FmaFloat(Scalar, Addend1.Blue, Addend0.Blue);

    Sum = Color3InitializeFromComponents(Red, Green, Blue);

    return Sum;
}

SFORCEINLINE
COLOR3
Color3Subtract(
    _In_ COLOR3 Minuend,
    _In_ COLOR3 Subtrahend
    )
{
    COLOR3 Difference;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    Red = Minuend.Red - Subtrahend.Red;
    Green = Minuend.Green - Subtrahend.Green;
    Blue = Minuend.Blue - Subtrahend.Blue;

    Difference = Color3InitializeFromComponents(Red, Green, Blue);

    return Difference;
}

SFORCEINLINE
COLOR3
Color3ScaleByColor(
    _In_ COLOR3 Color,
    _In_ COLOR3 Scalar
    )
{
    COLOR3 Scaled;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    Red = Color.Red * Scalar.Red;
    Green = Color.Green * Scalar.Green;
    Blue = Color.Blue * Scalar.Blue;

    Scaled = Color3InitializeFromComponents(Red, Green, Blue);

    return Scaled;
}

SFORCEINLINE
COLOR3
Color3ScaleByScalar(
    _In_ COLOR3 Color,
    _In_ FLOAT Scalar
    )
{
    COLOR3 Scaled;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(IsFiniteFloat(Scalar) != FALSE);

    Red = Color.Red * Scalar;
    Green = Color.Green * Scalar;
    Blue = Color.Blue * Scalar;

    Scaled = Color3InitializeFromComponents(Red, Green, Blue);

    return Scaled;
}

SFORCEINLINE
COLOR3
Color3DivideByColor(
    _In_ COLOR3 Dividend,
    _In_ COLOR3 Divisor
    )
{
    COLOR3 Quotient;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(IsNotZeroFloat(Divisor.Red) != FALSE);
    ASSERT(IsNotZeroFloat(Divisor.Green) != FALSE);
    ASSERT(IsNotZeroFloat(Divisor.Blue) != FALSE);

    Red = Dividend.Red / Divisor.Red;
    Green = Dividend.Green / Divisor.Green;
    Blue = Dividend.Blue / Divisor.Blue;

    Quotient = Color3InitializeFromComponents(Red, Green, Blue);

    return Quotient;
}

SFORCEINLINE
COLOR3
Color3DivideByScalar(
    _In_ COLOR3 Dividend,
    _In_ FLOAT Divisor
    )
{
    COLOR3 Quotient;
    FLOAT Inverse;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(IsFiniteFloat(Divisor) != FALSE);
    ASSERT(IsNotZeroFloat(Divisor) != FALSE);

    Inverse = (FLOAT) 1.0 / Divisor;

    Red = Dividend.Red * Inverse;
    Green = Dividend.Green * Inverse;
    Blue = Dividend.Blue * Inverse;

    Quotient = Color3InitializeFromComponents(Red, Green, Blue);

    return Quotient;
}

SFORCEINLINE
FLOAT
Color3AverageComponents(
    _In_ COLOR3 Color
    )
{
    FLOAT Average;

    Average = (Color.Red + Color.Green + Color.Blue) / (FLOAT) 3.0;

    return Average;
}

SFORCEINLINE
BOOL
Color3IsBlack(
    _In_ COLOR3 Color
    )
{
    BOOL Result;

    Result = IsZeroFloat(Color.Red) != FALSE &&
             IsZeroFloat(Color.Green) != FALSE &&
             IsZeroFloat(Color.Blue) != FALSE;

    return Result;
}

SFORCEINLINE
COLOR3
Color3InitializeBlack(
    VOID
    )
{
    COLOR3 Color;

    Color = Color3InitializeFromComponents((FLOAT) 0.0,
                                           (FLOAT) 0.0,
                                           (FLOAT) 0.0);

    return Color;
}

#endif // _IRIS_ADVANCED_COLOR3_