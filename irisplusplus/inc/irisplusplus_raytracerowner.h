/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_raytracerowner.h

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerOwner type.

--*/

#include <irisplusplus.h>

#ifndef _RAYTRACER_OWNER_IRIS_PLUS_PLUS_
#define _RAYTRACER_OWNER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracerOwner final {
public:
    IRISPLUSPLUSAPI
    RayTracerOwner(
        void
        );
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsOutOfOrder(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsInOrder(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    ~RayTracerOwner(
        void
        );

private:
    PRAYTRACER_OWNER Data;
};

} // namespace Iris

#endif // _RAYTRACER_OWNER_IRIS_PLUS_PLUS_