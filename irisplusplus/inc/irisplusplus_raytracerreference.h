/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_raytracerreference.h

Abstract:

    This file contains the definitions for the 
    Iris++ raytracer reference type.

--*/

#include <irisplusplus.h>

#ifndef _RAYTRACER_REFERENCE_IRIS_PLUS_PLUS_
#define _RAYTRACER_REFERENCE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracerReference final {
private:
    RayTracerReference(
        _In_ PRAYTRACER_REFERENCE RayTracerRef
        )
    : Data(RayTracerRef)
    { }
    
public:
    _Ret_
    PRAYTRACER_REFERENCE
    AsPRAYTRACER_REFERENCE(
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
        
        RayTracerReferenceGetRay(Data, &CurrentRay);
        
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
    PRAYTRACER_REFERENCE Data;
};

} // namespace Iris

#endif // _RAYTRACER_REFERENCE_IRIS_PLUS_PLUS_