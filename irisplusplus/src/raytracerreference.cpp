/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracerreference.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerReference type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Functions
//

void
RayTracerReference::Trace(
    _In_ const Shape & ShapeRef
    )
{
    ISTATUS Status = RayTracerReferenceTraceShape(Data, ShapeRef.AsPCSHAPE());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracerReference::Trace(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef
    )
{
    ISTATUS Status = RayTracerReferenceTracePremultipliedShapeWithTransform(Data,
                                                                            ShapeRef.AsPCSHAPE(),
                                                                            MatrixRef.AsPCMATRIX());
    
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracerReference::Trace(
    _In_ const Shape & ShapeRef,
    _In_ const Matrix & MatrixRef,
    _In_ bool Premultiplied
    )
{
    ISTATUS Status = RayTracerReferenceTraceShapeWithTransform(Data, 
                                                               ShapeRef.AsPCSHAPE(),
                                                               MatrixRef.AsPCMATRIX(),
                                                               Premultiplied ? TRUE : FALSE);
                                                      
    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(ISTATUSToCString(Status));
    }
}

} // namespace Iris