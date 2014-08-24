/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_point.h

Abstract:

    This file contains the definitions for the 
    Iris++ point type.

--*/

#include <irisplusplus.h>

#ifndef _POINT_IRIS_PLUS_PLUS_
#define _POINT_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Point final {
public:
    Point(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        )
    : Data(PointCreate(X, Y, Z))
    { }

    Point(
        _In_ const POINT3 & IrisPoint
        )
    : Data(IrisPoint)
    { }

    static
    Point
    AddScaled(
        _In_ const Point & Addend0, 
        _In_ const Vector & Addend1, 
        _In_ FLOAT Scalar
        )
    {
        POINT3 IrisAddend0;
        VECTOR3 IrisAddend1;
        POINT3 IrisResult;

        IrisAddend0 = Addend0.AsPOINT3();
        IrisAddend1 = Addend1.AsVECTOR3();

        IrisResult = PointVectorAddScaled(IrisAddend0, 
                                          IrisAddend1, 
                                          Scalar);

        return Point(IrisResult);
    }

    static
    Point
    SubtractScaled(
        _In_ const Point & Minuend, 
        _In_ const Vector & Subtrahend, 
        _In_ FLOAT Scalar
        )
    {
        POINT3 IrisMinuend;
        VECTOR3 IrisSubtrahend;
        POINT3 IrisResult;

        IrisMinuend = Minuend.AsPOINT3();
        IrisSubtrahend = Subtrahend.AsVECTOR3();

        IrisResult = PointVectorSubtractScaled(IrisMinuend, 
                                               IrisSubtrahend, 
                                               Scalar);

        return Point(IrisResult);
    }

    static
    Point
    InverseMultiply(
        _In_ const Matrix & Multiplicand0,
        _In_ const Point & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        POINT3 IrisMultiplicand1;
        POINT3 IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsPOINT3();

        IrisResult = PointMatrixInverseMultiply(IrisMultiplicand0,
                                                IrisMultiplicand1);

        return Point(IrisResult);
    }

    POINT3
    AsPOINT3(
        void
        ) const
    {
        return Data;
    }

    _Ret_
    PCPOINT3 
    AsPCPOINT3(
        void
        ) const
    {
        return &Data;
    }

    FLOAT 
    X(
        void
        ) const
    {
        return Data.X;
    }

    FLOAT 
    Y(
        void
        ) const
    {
        return Data.Y;
    }

    FLOAT 
    Z(
        void
        ) const
    {
        return Data.Z;
    }

    bool 
    Validate(
        void
        ) const
    {
        BOOL Valid;

        Valid = PointValidate(Data);

        return (Valid != FALSE) ? true : false;
    }

private:
    const POINT3 Data;
};

//
// Functions
//

static 
inline 
Point 
operator+(
    _In_ const Point & Addend0, 
    _In_ const Vector & Addend1
    )
{
    POINT3 IrisAddend0;
    VECTOR3 IrisAddend1;
    POINT3 IrisResult;

    IrisAddend0 = Addend0.AsPOINT3();
    IrisAddend1 = Addend1.AsVECTOR3();

    IrisResult = PointVectorAdd(IrisAddend0, IrisAddend1);

    return Point(IrisResult);
}

static
inline
Point 
operator-(
    _In_ const Point & Minuend, 
    _In_ const Vector & Subtrahend
    )
{
    POINT3 IrisMinuend;
    VECTOR3 IrisSubtrahend;
    POINT3 IrisResult;

    IrisMinuend = Minuend.AsPOINT3();
    IrisSubtrahend = Subtrahend.AsVECTOR3();

    IrisResult = PointVectorSubtract(IrisMinuend, IrisSubtrahend);

    return Point(IrisResult);
}

static
inline
Point
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Point & Multiplicand1
    )
{
    PCMATRIX IrisMultiplicand0;
    POINT3 IrisMultiplicand1;
    POINT3 IrisResult;

    IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
    IrisMultiplicand1 = Multiplicand1.AsPOINT3();

    IrisResult = PointMatrixMultiply(IrisMultiplicand0,
                                     IrisMultiplicand1);

    return Point(IrisResult);
}

} // namespace Iris

#endif // _POINT_IRIS_PLUS_PLUS_