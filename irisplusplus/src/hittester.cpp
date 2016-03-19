/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracerreference.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerReference type.

--*/

#include <irisplusplusp.h>

namespace Iris {

//
// Functions
//

void
HitTester::Test(
    _In_ const Shape & ShapeRef
    )
{
    ISTATUS Status = HitTesterTestShape(Data, ShapeRef.AsPCSHAPE());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
HitTester::Test(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef
    )
{
    ISTATUS Status = HitTesterTestPremultipliedShapeWithTransform(Data,
                                                                  ShapeRef.AsPCSHAPE(),
                                                                  MatrixRef.AsPCMATRIX());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
HitTester::Test(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef,
    _In_ bool Premultiplied
    )
{
    ISTATUS Status = HitTesterTestShapeWithTransform(Data, 
                                                     ShapeRef.AsPCSHAPE(),
                                                     MatrixRef.AsPCMATRIX(),
                                                     Premultiplied ? TRUE : FALSE);
                                                      
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

} // namespace Iris