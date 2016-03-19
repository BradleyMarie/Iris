/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_hittester.h

Abstract:

    This file contains the definitions for the 
    Iris++ raytracer reference type.

--*/

#include <irisplusplus.h>

#ifndef _HIT_TESTER_IRIS_PLUS_PLUS_
#define _HIT_TESTER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class HitTester final {
public:
    HitTester(
        _In_ PHIT_TESTER HitTesterPtr
        )
    : Data(HitTesterPtr)
    { }
    
    _Ret_
    PHIT_TESTER
    AsPHIT_TESTER(
        void
        )
    {
        return Data;
    }

    _Ret_
    IRISPLUSPLUSAPI
    Ray
    GetRay(
        void
        ) const
    {
        RAY CurrentRay;
        
        HitTesterGetRay(Data, &CurrentRay);
        
        return Ray(CurrentRay);
    }

    IRISPLUSPLUSAPI
    void
    Test(
        _In_ const Shape & ShapeRef
        );

    IRISPLUSPLUSAPI
    void
    Test(
        _In_ const Shape & ShapeRef,
        _In_ const Matrix & MatrixRef
        );

    IRISPLUSPLUSAPI
    void
    Test(
        _In_ const Shape & ShapeRef,
        _In_ const Matrix & MatrixRef,
        _In_ bool Premultiplied
        );

private:
    PHIT_TESTER Data;
};

} // namespace Iris

#endif // _HIT_TESTER_IRIS_PLUS_PLUS_