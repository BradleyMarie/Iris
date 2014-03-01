/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_color4.h

Abstract:

    This file contains the definitions for the COLOR4 type.

--*/

#ifndef _COLOR4_IRIS_SHADING_MODEL_
#define _COLOR4_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

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
VOID
Color4InitializeFromComponents(
    _Out_ PCOLOR4 Color,
    _In_ FLOAT Red,
    _In_ FLOAT Green,
    _In_ FLOAT Blue,
    _In_ FLOAT Alpha
    )
{
    ASSERT(Color != NULL);
    ASSERT(IsNormalFloat(Red));
    ASSERT(IsFiniteFloat(Red));
    ASSERT(IsNormalFloat(Green));
    ASSERT(IsFiniteFloat(Green));
    ASSERT(IsNormalFloat(Blue));
    ASSERT(IsFiniteFloat(Blue));
    ASSERT(IsNormalFloat(Alpha));
    ASSERT(IsFiniteFloat(Alpha));
    ASSERT((FLOAT) 0.0 <= Alpha);
    ASSERT(Alpha <= (FLOAT) 1.0);

    Color->Red = Red;
    Color->Green = Green;
    Color->Blue = Blue;
    Color->Alpha = Alpha;
}

SFORCEINLINE
VOID
Color4InitializeFromColor3(
    _Out_ PCOLOR4 Color,
    _In_ PCCOLOR3 Color3,
    _In_ FLOAT Alpha
    )
{
    ASSERT(Color != NULL);
    ASSERT(Color3 != NULL);
    ASSERT((FLOAT) 0.0 <= Alpha);
    ASSERT(Alpha <= (FLOAT) 1.0);

    Color4InitializeFromComponents(Color,
                                   Color3->Red,
                                   Color3->Green,
                                   Color3->Blue,
                                   Alpha);
}

SFORCEINLINE
VOID
Color3InitializeFromColor4(
    _Out_ PCOLOR3 Color,
    _In_ PCCOLOR4 Color4
    )
{
    ASSERT(Color != NULL);
    ASSERT(Color4 != NULL);

    Color3InitializeFromComponents(Color,
                                   Color4->Red,
                                   Color4->Green,
                                   Color4->Blue);
}

SFORCEINLINE
VOID
Color4Add(
    _In_ PCCOLOR4 Addend0,
    _In_ PCCOLOR4 Addend1,
    _Out_ PCOLOR4 Sum
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    ASSERT(Addend0 != NULL);
    ASSERT(Addend1 != NULL);
    ASSERT(Sum != NULL);

    Red = Addend0->Red + Addend1->Red;
    Green = Addend0->Green + Addend1->Green;
    Blue = Addend0->Blue + Addend1->Blue;
    Alpha = Addend0->Alpha + Addend1->Alpha;

    Color4InitializeFromComponents(Sum, Red, Green, Blue, Alpha);
}

SFORCEINLINE
VOID
Color4ScaleByColor(
    _In_ PCCOLOR4 Color,
    _In_ PCCOLOR4 Scalar,
    _Out_ PCOLOR4 Scaled
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    ASSERT(Color != NULL);
    ASSERT(Scalar != NULL);
    ASSERT(Scaled != NULL);

    Red = Color->Red * Scalar->Red;
    Green = Color->Green * Scalar->Green;
    Blue = Color->Blue * Scalar->Blue;
    Alpha = Color->Alpha * Scalar->Alpha;

    Color4InitializeFromComponents(Scaled, Red, Green, Blue, Alpha);
}

SFORCEINLINE
VOID
Color4ScaleByScalar(
    _In_ PCCOLOR4 Color,
    _In_ FLOAT Scalar,
    _Out_ PCOLOR4 Scaled
    )
{
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    ASSERT(Color != NULL);
    ASSERT(IsNormalFloat(Scalar));
    ASSERT(IsFiniteFloat(Scalar));
    ASSERT(Scaled != NULL);

    Red = Color->Red * Scalar;
    Green = Color->Green * Scalar;
    Blue = Color->Blue * Scalar;
    Alpha = Color->Alpha * Alpha;

    Color4InitializeFromComponents(Scaled, Red, Green, Blue, Alpha);
}

SFORCEINLINE
VOID
Color4Over(
    _In_ PCCOLOR4 Over,
    _In_ PCCOLOR4 Under,
    _Out_ PCOLOR4 Blended
    )
{
    FLOAT AlphaScalar;
    FLOAT Red;
    FLOAT Green;
    FLOAT Blue;
    FLOAT Alpha;

    ASSERT(Over != NULL);
    ASSERT(Under != NULL);
    ASSERT(Blended != NULL);

    AlphaScalar = Under->Alpha * ((FLOAT) 1.0 - Over->Alpha);

    Red = Over->Red * Over->Alpha + Under->Red * AlphaScalar;
    Green = Over->Green * Over->Alpha + Under->Green * AlphaScalar;
    Blue = Over->Blue * Over->Alpha + Under->Blue * AlphaScalar;
    Alpha = MinFloat(Over->Alpha + AlphaScalar, (FLOAT) 1.0);

    Color4InitializeFromComponents(Blended, Red, Green, Blue, Alpha);
}

SFORCEINLINE
VOID
Color4InitializeTransparent(
    _Out_ PCOLOR4 Color
    )
{
    ASSERT(Color != NULL);

    Color4InitializeFromComponents(Color,
                                   (FLOAT) 0.0,
                                   (FLOAT) 0.0,
                                   (FLOAT) 0.0,
                                   (FLOAT) 0.0);
}

#endif // _COLOR4_IRIS_SHADING_MODEL_