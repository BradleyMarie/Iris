/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_ray.h

Abstract:

    This file contains the definitions for the 
    Iris++ ray type.

--*/

#include <irisplusplus.h>

#ifndef _RAY_IRIS_PLUS_PLUS_
#define _RAY_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Ray {
public:
    Ray(
        _In_ const Point & Origin, 
        _In_ const Vector & Direction
        )
        : Data(RayCreate(Origin.AsPOINT3(), Direction.AsVECTOR3()))
    { }

    Ray(
        _In_ RAY IrisRay
        )
        : Data(IrisRay)
    { }

    Point 
    Endpoint(
        _In_ FLOAT Distance
        ) const
    {
        POINT3 IrisEndpoint;

        IrisEndpoint = RayEndpoint(Data, Distance);

        return Point(IrisEndpoint);
    }

    RAY 
    AsRAY(
        void
        ) const
    {
        return Data;
    }

    Point
    Origin(
        void
        ) const
    {
        return Point(Data.Origin);
    }

    Vector 
    Direction(
        void
        ) const
    {
        return Vector(Data.Direction);
    }

    bool 
    Validate(
        void
        ) const
    {
        BOOL Valid;

        Valid = RayValidate(Data);

        return (Valid != FALSE) ? true : false;
    }

    static 
    Ray 
    Normalize(
        _In_ const Ray & ToNormalize
        )
    {
        RAY IrisResult;

        IrisResult = RayNormalize(ToNormalize.Data);

        return Ray(IrisResult);
    }

    static
    Ray
    InverseMultiply(
        _In_ const Matrix & Multiplicand0,
        _In_ const Ray & Multiplicand1
        )
    {
        PCMATRIX IrisMultiplicand0;
        RAY IrisMultiplicand1;
        RAY IrisResult;

        IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
        IrisMultiplicand1 = Multiplicand1.AsRAY();

        IrisResult = RayMatrixInverseMultiply(IrisMultiplicand0,
                                              IrisMultiplicand1);

        return Ray(IrisResult);
    }

private:
    const RAY Data;
};

static
inline
Ray
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Ray & Multiplicand1
    )
{
    PCMATRIX IrisMultiplicand0;
    RAY IrisMultiplicand1;
    RAY IrisResult;

    IrisMultiplicand0 = Multiplicand0.AsPCMATRIX();
    IrisMultiplicand1 = Multiplicand1.AsRAY();

    IrisResult = RayMatrixMultiply(IrisMultiplicand0,
                                   IrisMultiplicand1);

    return Ray(IrisResult);
}

} // namespace Iris

#endif // _RAY_IRIS_PLUS_PLUS_