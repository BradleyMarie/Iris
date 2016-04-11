/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_raytracer.h

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracer type.

--*/

#include <irisplusplus.h>

#ifndef _RAYTRACER_IRIS_PLUS_PLUS_
#define _RAYTRACER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracer final {
public:
    RayTracer(
        _In_ RayTracer && ToMove
        )
        : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    static
    RayTracer
    Create(
        void
        )
    {
        return RayTracer();
    }

    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsOutOfOrder(
        _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsInOrder(
        _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
        );
        
    RayTracer(
        _In_ RayTracer & ToCopy
        ) = delete;
        
    RayTracer &
    operator=(
        _In_ const RayTracer & ToCopy
        ) = delete;
    
    ~RayTracer(
        void
        )
     {
         RayTracerFree(Data);
     }

private:
    PRAYTRACER Data;

    RayTracer(
        void
        )
    {
        ISTATUS Status = RayTracerAllocate(&Data);
        
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();;
        }
    }

    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS 
    TestGeometryAdapter(
        _In_opt_ PCVOID Context,
        _Inout_ PHIT_TESTER HitTesterPtr,
        _In_ RAY WorldRay
        );

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
        _In_ PCMATRIX ModelToWorldReference,
        _In_ VECTOR3 ModelViewer,
        _In_ POINT3 ModelHitPoint,
        _In_ POINT3 WorldHitPoint
        );
};

} // namespace Iris

#endif // _RAYTRACER_IRIS_PLUS_PLUS_