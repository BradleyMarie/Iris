/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_math.h

Abstract:

    This module implements the Iris math routines.

--*/

#include <iris.h>
#include <math.h>

#ifndef _MATH_IRIS_
#define _MATH_IRIS_

#define PI_FLOAT (FLOAT)3.14159265358979323846264338327950288419716939
#define INV_PI_FLOAT (FLOAT)0.3183098861837906715377675267450287240689

#define SqrtFloat(number) sqrtf(number)
#define AbsFloat(number) fabsf(number)
#define CosFloat(number) cosf(number)
#define SinFloat(number) sinf(number)
#define IsNaNFloat(number) isnan(number)
#define IsInfFloat(number) isinf(number)
#define IsZeroFloat(number) (number == (FLOAT) 0.0)

#if __STDC_VERSION__ >= 199901L

#define IsNormalFloat(number) isnormal(number)
#define IsFiniteFloat(number) isfinite(number)
#define MaxFloat(number0, number1) fmaxf(number0, number1)
#define MinFloat(number0, number1) fminf(number0, number1)
#define FmaFloat(m0, m1, a0) fmaf(m0, m1, a0)

#else

#define IsNormalFloat(number) TRUE
#define IsFiniteFloat(number) TRUE
#define MaxFloat(number0, number1) ((number0 < number1) ? number1 : number0)
#define MinFloat(number0, number1) ((number0 < number1) ? number0 : number1)
#define FmaFloat(m0, m1, a0) ((m0 * m1) + a0)

#endif

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedMultiplySizeT(
    _In_ SIZE_T Multiplicand0,
    _In_ SIZE_T Multiplicand1,
    _Out_ PSIZE_T Product
    )
{
    SIZE_T RawProduct;

    if (Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RawProduct = Multiplicand0 * Multiplicand1;

    if (Multiplicand0 != 0 &&
        RawProduct / Multiplicand0 != Multiplicand1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Product = RawProduct;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedMultiplyUInt64(
    _In_ UINT64 Multiplicand0,
    _In_ UINT64 Multiplicand1,
    _Out_ PUINT64 Product
    )
{
    UINT64 RawProduct;

    if (Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RawProduct = Multiplicand0 * Multiplicand1;

    if (Multiplicand0 != 0 &&
        RawProduct / Multiplicand0 != Multiplicand1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Product = RawProduct;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedMultiplyUInt8(
    _In_ UINT8 Multiplicand0,
    _In_ UINT8 Multiplicand1,
    _Out_ PUINT8 Product
    )
{
    UINT16 RawProduct;

    if (Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RawProduct = (UINT16) Multiplicand0 * (UINT16) Multiplicand1;

    if (RawProduct > UINT8_MAX)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Product = (UINT8) RawProduct;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedMultiplyUInt16(
    _In_ UINT16 Multiplicand0,
    _In_ UINT16 Multiplicand1,
    _Out_ PUINT16 Product
    )
{
    UINT32 RawProduct;

    if (Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RawProduct = (UINT32) Multiplicand0 * (UINT32) Multiplicand1;

    if (RawProduct > UINT16_MAX)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Product = (UINT16) RawProduct;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedMultiplyUInt32(
    _In_ UINT32 Multiplicand0,
    _In_ UINT32 Multiplicand1,
    _Out_ PUINT32 Product
    )
{
    UINT64 RawProduct;

    if (Product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RawProduct = (UINT64) Multiplicand0 * (UINT64) Multiplicand1;

    if (RawProduct > UINT32_MAX)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Product = (UINT32) RawProduct;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedAddSizeT(
    _In_ SIZE_T Addend0,
    _In_ SIZE_T Addend1,
    _Out_ PSIZE_T Sum
    )
{
    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (SIZE_T_MAX - Addend0 < Addend1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Sum = Addend0 + Addend1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedAddUInt64(
    _In_ UINT64 Addend0,
    _In_ UINT64 Addend1,
    _Out_ PUINT64 Sum
    )
{
    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (UINT64_MAX - Addend0 < Addend1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Sum = Addend0 + Addend1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedAddUInt32(
    _In_ UINT32 Addend0,
    _In_ UINT32 Addend1,
    _Out_ PUINT32 Sum
    )
{
    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (UINT32_MAX - Addend0 < Addend1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Sum = Addend0 + Addend1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedAddUInt16(
    _In_ UINT16 Addend0,
    _In_ UINT16 Addend1,
    _Out_ PUINT16 Sum
    )
{
    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (UINT16_MAX - Addend0 < Addend1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Sum = Addend0 + Addend1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
CheckedAddUInt8(
    _In_ UINT8 Addend0,
    _In_ UINT8 Addend1,
    _Out_ PUINT8 Sum
    )
{
    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    if (UINT8_MAX - Addend0 < Addend1)
    {
        return ISTATUS_INTEGER_OVERFLOW;
    }

    *Sum = Addend0 + Addend1;

    return ISTATUS_SUCCESS;
}

#endif // _MATH_IRIS_