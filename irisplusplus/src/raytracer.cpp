/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracer.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracer type.

--*/

#include <irisplusplusp.h>

namespace Iris {

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RayTracer::TestGeometryAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTesterPtr,
    _In_ RAY WorldRay
    )
{
    auto TestGeometryRoutine = static_cast<const std::function<void(HitTester, Ray)> *>(Context);
    
    (*TestGeometryRoutine)(HitTester(HitTesterPtr), Ray(WorldRay));

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RayTracer::ProcessHitsAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> *>(Context);
    
    bool Stop = (*ProcessHitRoutine)(Hit->Data,
                                     Hit->Distance,
                                     Hit->FaceHit,
                                     Hit->AdditionalData,
                                     Hit->AdditionalDataSizeInBytes);

    if (Stop)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RayTracer::ProcessHitsWithCoordinatesAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> *>(Context);

    bool Stop = (*ProcessHitRoutine)(Hit->Data,
                                     Hit->Distance,
                                     Hit->FaceHit,
                                     Hit->AdditionalData,
                                     Hit->AdditionalDataSizeInBytes,
                                     MatrixReference(ModelToWorld),
                                     Vector(ModelViewer),
                                     Point(ModelHitPoint),
                                     Point(WorldHitPoint));

    if (Stop)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    return ISTATUS_SUCCESS;
}

void
RayTracer::TraceClosestHit(
    _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessClosestHit(Data,
                                                          WorldRay.AsRAY(),
                                                          MinimumDistance,
                                                          TestGeometryAdapter,
                                                          &TestGeometryRoutine,
                                                          ProcessHitsAdapter,
                                                          &ProcessHitRoutine);
    
    if (Status == ISTATUS_SUCCESS)
    {
        return;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracer::TraceClosestHit(
    _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessClosestHitWithCoordinates(Data,
                                                                         WorldRay.AsRAY(),
                                                                         MinimumDistance,
                                                                         TestGeometryAdapter,
                                                                         &TestGeometryRoutine,
                                                                         ProcessHitsWithCoordinatesAdapter,
                                                                         &ProcessHitRoutine);

    if (Status == ISTATUS_SUCCESS)
    {
        return;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracer::TraceAllHitsOutOfOrder(
    _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(Data,
                                                                 WorldRay.AsRAY(),
                                                                 TestGeometryAdapter,
                                                                 &TestGeometryRoutine,
                                                                 ProcessHitsAdapter,
                                                                 &ProcessHitRoutine);

    if (Status == ISTATUS_SUCCESS)
    {
        return;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

void
RayTracer::TraceAllHitsInOrder(
    _In_ std::function<void(HitTester, Ray)> TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(Data,
                                                                             WorldRay.AsRAY(),
                                                                             TestGeometryAdapter,
                                                                             &TestGeometryRoutine,
                                                                             ProcessHitsWithCoordinatesAdapter,
                                                                             &ProcessHitRoutine);

    if (Status == ISTATUS_SUCCESS)
    {
        return;
    }

    switch (Status)
    {
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
    }
}

} // namespace Iris