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
        _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsOutOfOrder(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsInOrder(
        _In_ const Scene & Scene,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    ~RayTracerOwner(
        void
        );

private:
    PRAYTRACER_OWNER Data;
    
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS 
    ProcessHitsAdapter(
        _Inout_opt_ PVOID Context, 
        _In_ PCHIT Hit
        );
        
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS 
    ProcessHitsWithCoordinatesAdapter(
        _Inout_opt_ PVOID Context, 
        _In_ PCHIT Hit,
        _In_ PCMATRIX_REFERENCE ModelToWorldReference,
        _In_ VECTOR3 ModelViewer,
        _In_ POINT3 ModelHitPoint,
        _In_ POINT3 WorldHitPoint
        );
};

} // namespace Iris

#endif // _RAYTRACER_OWNER_IRIS_PLUS_PLUS_