/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_color3.h

Abstract:

    This file contains the definitions for the COLOR3 type.

--*/

#ifndef _COLOR3_IRIS_SHADING_MODEL_
#define _COLOR3_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

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
VOID
Color3InitializeFromComponents(
    _Out_ PCOLOR3 Color,
    _In_ FLOAT Red,
    _In_ FLOAT Green,
    _In_ FLOAT Blue
    )
{
    ASSERT(Color != NULL);
    ASSERT(IsNormalFloat(Red));
    ASSERT(IsFiniteFloat(Red));
    ASSERT(IsNormalFloat(Green));
    ASSERT(IsFiniteFloat(Green));
    ASSERT(IsNormalFloat(Blue));
    ASSERT(IsFiniteFloat(Blue));

    Color->Red = Red;
    Color->Green = Green;
    Color->Blue = Blue;
}

SFORCEINLINE
VOID
Color3Add(
    _In_ PCCOLOR3 Addend0,
    _In_ PCCOLOR3 Addend1,
    _Out_ PCOLOR3 Sum
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Addend0 != NULL);
    ASSERT(Addend1 != NULL);
    ASSERT(Sum != NULL);

    Red = Addend0->Red + Addend1->Red;
    Green = Addend0->Green + Addend1->Green;
    Blue = Addend0->Blue + Addend1->Blue;

    Color3InitializeFromComponents(Sum, Red, Green, Blue);
}

SFORCEINLINE
VOID
Color3Subtract(
    _In_ PCCOLOR3 Minuend,
    _In_ PCCOLOR3 Subtrahend,
    _Out_ PCOLOR3 Difference
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Minuend != NULL);
    ASSERT(Subtrahend != NULL);
    ASSERT(Difference != NULL);

    Red = Minuend->Red - Difference->Red;
    Green = Minuend->Green - Difference->Green;
    Blue = Minuend->Blue - Difference->Blue;

    Color3InitializeFromComponents(Difference, Red, Green, Blue);
}

SFORCEINLINE
VOID
Color3ScaleByColor(
    _In_ PCCOLOR3 Color,
    _In_ PCCOLOR3 Scalar,
    _Out_ PCOLOR3 Scaled
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Color != NULL);
    ASSERT(Scalar != NULL);
    ASSERT(Scaled != NULL);

    Red = Color->Red * Scalar->Red;
    Green = Color->Green * Scalar->Green;
    Blue = Color->Blue * Scalar->Blue;

    Color3InitializeFromComponents(Scaled, Red, Green, Blue);
}

SFORCEINLINE
VOID
Color3ScaleByScalar(
    _In_ PCCOLOR3 Color,
    _In_ FLOAT Scalar,
    _Out_ PCOLOR3 Scaled
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Color != NULL);
    ASSERT(IsNormalFloat(Scalar));
    ASSERT(IsFiniteFloat(Scalar));
    ASSERT(Scaled != NULL);

    Red = Color->Red * Scalar;
    Green = Color->Green * Scalar;
    Blue = Color->Blue * Scalar;

    Color3InitializeFromComponents(Scaled, Red, Green, Blue);
}

SFORCEINLINE
VOID
Color3DivideByColor(
    _In_ PCCOLOR3 Dividend,
    _In_ PCCOLOR3 Divisor,
    _Out_ PCOLOR3 Quotient
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Dividend != NULL);
    ASSERT(Divisor != NULL);
    ASSERT(IsZeroFloat(Divisor->Red));
    ASSERT(IsZeroFloat(Divisor->Green));
    ASSERT(IsZeroFloat(Divisor->Blue));
    ASSERT(Quotient != NULL);

    Red = Dividend->Red / Divisor->Red;
    Green = Dividend->Green / Divisor->Green;
    Blue = Dividend->Blue / Divisor->Blue;

    Color3InitializeFromComponents(Quotient, Red, Green, Blue);
}

SFORCEINLINE
VOID
Color3DivideByScalar(
    _In_ PCCOLOR3 Dividend,
    _In_ FLOAT Divisor,
    _Out_ PCOLOR3 Quotient
    )
{
    FLOAT Inverse;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;

    ASSERT(Dividend != NULL);
    ASSERT(IsNormalFloat(Divosor));
    ASSERT(IsFiniteFloat(Divisor));
    ASSERT(IsZeroFloat(Divisor));
    ASSERT(Quotient != NULL);

    Inverse = (FLOAT) 1.0 / Divisor;

    Red = Dividend->Red * Inverse;
    Green = Dividend->Green * Inverse;
    Blue = Dividend->Blue * Inverse;

    Color3InitializeFromComponents(Quotient, Red, Green, Blue);
}

SFORCEINLINE
BOOL
Color3IsBlack(
    _In_ PCCOLOR3 Color
    )
{
    BOOL Result;

    ASSERT(Color != NULL);

    Result = IsZeroFloat(Color->Red) &&
             IsZeroFloat(Color->Green) &&
             IsZeroFloat(Color->Blue);

    return Result;
}

#endif // _COLOR3_IRIS_SHADING_MODEL_