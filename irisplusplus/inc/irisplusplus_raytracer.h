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
    static
    RayTracer
    Create(
        void
        )
    {
        return RayTracer();
    }

    typedef std::function<bool(PCVOID, FLOAT, UINT32, UINT32, PCVOID, SIZE_T)> ProcessHitsFunction;
    typedef std::function<bool(PCVOID, FLOAT, UINT32, UINT32, PCVOID, SIZE_T, MatrixReference, const Vector &, const Point &, const Point &)> ProcessHitsWithCoordinatesFunction;
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ ProcessHitsFunction ProcessHitsRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceClosestHit(
        _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ ProcessHitsWithCoordinatesFunction ProcessHitsRoutine
        );
    
    IRISPLUSPLUSAPI
    void
    TraceAllHitsOutOfOrder(
        _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ ProcessHitsWithCoordinatesFunction ProcessHitRoutine
        );

    IRISPLUSPLUSAPI
    void
    TraceHitsInOrder(
        _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ FLOAT MinimumDistance,
        _In_ ProcessHitsWithCoordinatesFunction ProcessHitRoutine
        );

    IRISPLUSPLUSAPI
    void
    TraceAllHitsInOrder(
        _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
        _In_ const Ray & WorldRay,
        _In_ ProcessHitsWithCoordinatesFunction ProcessHitRoutine
        );

    RayTracer(
        _In_ const RayTracer & ToCopy
        )
    {
        ISTATUS Status = RayTracerAllocate(&Data);
        
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();;
        }
    }
    
    RayTracer &
    operator=(
        _In_ const RayTracer & ToCopy
        ) noexcept
    {
        return *this;
    }
    
    ~RayTracer(
        void
        ) noexcept
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
};

} // namespace Iris

#endif // _RAYTRACER_IRIS_PLUS_PLUS_
