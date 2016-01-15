/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracer.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracer type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// Functions
//

void
RayTracer::Trace(
    _In_ PCSHAPE ShapePtr
    )
{
    ISTATUS Status = RayTracerTraceShape(Data, ShapePtr);
    
    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

void
RayTracer::Trace(
    _In_ PCSHAPE ShapePtr,
    _In_ PCMATRIX MatrixPtr
    )
{
    ISTATUS Status = RayTracerTracePremultipliedShapeWithTransform(Data, 
                                                                   ShapePtr,
                                                                   MatrixPtr);
    
    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

void
RayTracer::Trace(
    _In_ PCSHAPE ShapePtr,
    _In_ PCMATRIX MatrixPtr,
    _In_ BOOL Premultiplied
    )
{
    ISTATUS Status = RayTracerTraceShapeWithTransform(Data, 
                                                      ShapePtr,
                                                      MatrixPtr,
                                                      Premultiplied);
                                                      
    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }
}

} // namespace Iris