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
        _In_ FLOAT R,
        _In_ FLOAT G,
        _In_ FLOAT B
        ) noexcept
    : Data(Color3InitializeFromComponents(R, G, B))
    { }

    Color3(
        _In_ const Color3 & Color
        ) noexcept
    : Data(Color.AsCOLOR3())
    { }

    Color3(
        _In_ const COLOR3 & Color
        ) noexcept
    : Data(Color)
    { }

    static
    Color3
    CreateBlack(
        void
        ) noexcept
    {
        return Color3(Color3InitializeBlack());
    }

    COLOR3
    AsCOLOR3(
        void
        ) const noexcept
    {
        return Data;
    }
    
    bool
    Validate(
        void
        ) noexcept
    {
        BOOL Valid = Color3Validate(Data);

        return (Valid != FALSE) ? true : false;
    }

    bool
    IsBlack(
        void
        ) const noexcept
    {
        BOOL Black = Color3IsBlack(Data);

        return (Black != FALSE) ? true : false;
    }

    FLOAT
    AverageComponents(
        void
        ) const noexcept
    {
        return Color3AverageComponents(Data);
    }

    static
    Color3
    Fma(
        _In_ const Color3 & Addend0,
        _In_ const Color3 & Addend1,
        _In_ FLOAT Scalar
        ) noexcept
    {
        COLOR3 Result = Color3AddScaled(Addend0.Data,
                                        Addend1.Data,
                                        Scalar);

        return Color3(Result);
    }

    FLOAT 
    Red(
        void
        ) const noexcept
    {
        return Data.Red;
    }

    FLOAT 
    Green(
        void
        ) const noexcept
    {
        return Data.Green;
    }

    FLOAT 
    Blue(
        void
        ) const noexcept
    {
        return Data.Blue;
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
    ) noexcept
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
    ) noexcept
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
    ) noexcept
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
    ) noexcept
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
    ) noexcept
{
    COLOR3 Result = Color3DivideByScalar(Dividend.AsCOLOR3(),
                                         Divisor);

    return Color3(Result);
}

} // namespace IrisAdvanced

#endif // _COLOR3_IRIS_ADVANCED_PLUS_PLUS_