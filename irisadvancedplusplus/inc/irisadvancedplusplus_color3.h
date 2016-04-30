/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_color3.h

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ Color3 type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _COLOR3_IRIS_ADVANCED_PLUS_PLUS_
#define _COLOR3_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class Color3 final {
public:
    Color3(
        _In_ const Color3 & Color
        )
    : Data(Color.AsCOLOR3())
    { }

    Color3(
        _In_ const COLOR3 & Color
        )
    : Data(Color)
    { }

    static
    Color3
    CreateBlack(
        void
        )
    {
        return Color3(Color3InitializeBlack());
    }

    COLOR3
    AsCOLOR3(
        void
        ) const
    {
        return Data;
    }
    
    bool
    Validate(
        void
        )
    {
        BOOL Valid = Color3Validate(Data);

        return (Valid != FALSE) ? true : false;
    }

    bool
    IsBlack(
        void
        ) const
    {
        BOOL Black = Color3IsBlack(Data);

        return (Black != FALSE) ? true : false;
    }

    FLOAT
    AverageComponents(
        void
        ) const
    {
        return Color3AverageComponents(Data);
    }

    static
    Color3
    Fma(
        _In_ const Color3 & Addend0,
        _In_ const Color3 & Addend1,
        _In_ FLOAT Scalar
        )
    {
        COLOR3 Result = Color3AddScaled(Addend0.Data,
                                        Addend1.Data,
                                        Scalar);

        return Color3(Result);
    }

private:
    COLOR3 Data;
};

static
inline
Color3
operator+(
    _In_ const Color3 & Addend0,
    _In_ const Color3 & Addend1
    )
{
    COLOR3 Result = Color3Add(Addend0.AsCOLOR3(),
                              Addend1.AsCOLOR3());

    return Color3(Result);
}

static
inline
Color3
operator*(
    _In_ const Color3 & Multiplicand0,
    _In_ const Color3 & Multiplicand1
    )
{
    COLOR3 Result = Color3ScaleByColor(Multiplicand0.AsCOLOR3(),
                                       Multiplicand1.AsCOLOR3());

    return Color3(Result);
}

static
inline
Color3
operator*(
    _In_ const Color3 & Multiplicand,
    _In_ FLOAT Scalar
    )
{
    COLOR3 Result = Color3ScaleByScalar(Multiplicand.AsCOLOR3(),
                                        Scalar);

    return Color3(Result);
}

static
inline
Color3
operator/(
    _In_ const Color3 & Dividend,
    _In_ const Color3 & Divisor
    )
{
    COLOR3 Result = Color3DivideByColor(Dividend.AsCOLOR3(),
                                        Divisor.AsCOLOR3());

    return Color3(Result);
}

static
inline
Color3
operator/(
    _In_ const Color3 & Dividend,
    _In_ FLOAT Divisor
    )
{
    COLOR3 Result = Color3DivideByScalar(Dividend.AsCOLOR3(),
                                         Divisor);

    return Color3(Result);
}

} // namespace IrisAdvanced

#endif // _COLOR3_IRIS_ADVANCED_PLUS_PLUS_