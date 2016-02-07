/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracer.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracer type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Functions
//

void
RayTracer::Trace(
    _In_ const Shape & ShapeRef
    )
{
    ISTATUS Status = RayTracerTraceShape(Data, ShapeRef.AsPCSHAPE());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracer::Trace(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef
    )
{
    ISTATUS Status = RayTracerTracePremultipliedShapeWithTransform(Data,
                                                                   ShapeRef.AsPCSHAPE(),
                                                                   MatrixRef.AsPCMATRIX());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracer::Trace(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef,
    _In_ bool Premultiplied
    )
{
    ISTATUS Status = RayTracerTraceShapeWithTransform(Data, 
                                                      ShapeRef.AsPCSHAPE(),
                                                      MatrixRef.AsPCMATRIX(),
                                                      Premultiplied ? TRUE : FALSE);
                                                      
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

} // namespace Iris