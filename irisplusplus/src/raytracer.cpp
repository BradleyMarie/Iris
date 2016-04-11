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
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
TestGeometryAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTesterPtr,
    _In_ RAY WorldRay
    )
{
    assert(Context != nullptr);
    assert(HitTesterPtr != nullptr);
    assert(RayValidate(WorldRay) != FALSE);

    auto TestGeometryRoutine = static_cast<const std::function<void(HitTester, const Ray &)> *>(Context);
    
    (*TestGeometryRoutine)(HitTester(HitTesterPtr), Ray(WorldRay));

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
ProcessHitsAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit
    )
{
    assert(Context != nullptr);
    assert(Hit != nullptr);

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
static
ISTATUS 
ProcessHitsWithCoordinatesAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    assert(Context != nullptr);
    assert(Hit != nullptr);
    assert(VectorValidate(ModelViewer) != FALSE);
    assert(PointValidate(ModelHitPoint) != FALSE);
    assert(PointValidate(WorldHitPoint) != FALSE);

    auto ProcessHitRoutine = static_cast<std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, const Vector &, const Point &, const Point &)> *>(Context);

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

//
// Functions
//

void
RayTracer::TraceClosestHit(
    _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
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
    _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, const Vector &, const Point &, const Point &)> ProcessHitRoutine
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
    _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, const Vector &, const Point &, const Point &)> ProcessHitRoutine
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
    _In_ const std::function<void(HitTester, const Ray &)> & TestGeometryRoutine,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCVOID, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, const Vector &, const Point &, const Point &)> ProcessHitRoutine
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