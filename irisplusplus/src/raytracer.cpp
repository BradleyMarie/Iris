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
ISTATUS 
RayTracer::ProcessHitsAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T)> *>(Context);
    
    bool Stop = (*ProcessHitRoutine)(ShapeReference(Hit->ShapeReference),
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
    auto ProcessHitRoutine = static_cast<std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> *>(Context);

    bool Stop = (*ProcessHitRoutine)(ShapeReference(Hit->ShapeReference),
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

RayTracer::RayTracer(
    void
    )
{
    ISTATUS Status = RayTracerAllocate(&Data);
    
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
}

void
RayTracer::TraceClosestHit(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessClosestHit(Data,
                                                          Scene.AsPCSCENE(),
                                                          WorldRay.AsRAY(),
                                                          MinimumDistance,
                                                          ProcessHitsAdapter,
                                                          &ProcessHitRoutine);
    
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("WorldRay");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("MinimumDistance");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
            break;
    }
}

void
RayTracer::TraceClosestHit(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessClosestHitWithCoordinates(Data,
                                                                         Scene.AsPCSCENE(),
                                                                         WorldRay.AsRAY(),
                                                                         MinimumDistance,
                                                                         ProcessHitsWithCoordinatesAdapter,
                                                                         &ProcessHitRoutine);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("WorldRay");
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("MinimumDistance");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
            break;
    }
}

void
RayTracer::TraceAllHitsOutOfOrder(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(Data,
                                                                 Scene.AsPCSCENE(),
                                                                 WorldRay.AsRAY(),
                                                                 ProcessHitsAdapter,
                                                                 &ProcessHitRoutine);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("WorldRay");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
            break;
    }
}

void
RayTracer::TraceAllHitsInOrder(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(ShapeReference, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(Data,
                                                                             Scene.AsPCSCENE(),
                                                                             WorldRay.AsRAY(),
                                                                             ProcessHitsWithCoordinatesAdapter,
                                                                             &ProcessHitRoutine);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("WorldRay");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            throw std::runtime_error(ISTATUSToCString(Status));
            break;
    }
}

} // namespace Iris