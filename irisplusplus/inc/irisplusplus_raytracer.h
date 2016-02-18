/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_raytracer.h

Abstract:

    This file contains the definitions for the 
    Iris++ raytracer type.

--*/

#include <irisplusplus.h>

#ifndef _RAYTRACER_IRIS_PLUS_PLUS_
#define _RAYTRACER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracer final {
private:
    RayTracer(
        _In_ PRAYTRACER RayTracerPtr
        )
    : Data(RayTracerPtr)
    { }
    
public:
    _Ret_
    PRAYTRACER
    AsPRAYTRACER(
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
        
        RayTracerGetRay(Data, &CurrentRay);
        
        return Ray(CurrentRay);
    }

    IRISPLUSPLUSAPI
    void
    Trace(
        _In_ const Shape & ShapeRef
        );

    IRISPLUSPLUSAPI
    void
    Trace(
        _In_ const Shape & ShapeRef,
        _In_ const Matrix & MatrixRef
        );

    IRISPLUSPLUSAPI
    void
    Trace(
        _In_ const Shape & ShapeRef,
        _In_ const Matrix & MatrixRef,
        _In_ bool Premultiplied
        );

private:
    PRAYTRACER Data;
    
    friend class RayTracerOwner;
};

} // namespace Iris

#endif // _RAYTRACER_IRIS_PLUS_PLUS_