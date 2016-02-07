/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracerowner.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerOwner type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RayTracerOwner::ProcessHitsAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T)> *>(Context);
    
    bool Stop = (*ProcessHitRoutine)(ShapeHit->Shape,
                                     ShapeHit->Distance,
                                     ShapeHit->FaceHit,
                                     ShapeHit->AdditionalData,
                                     ShapeHit->AdditionalDataSizeInBytes);

    if (Stop)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
RayTracerOwner::ProcessHitsWithCoordinatesAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCSHAPE_HIT ShapeHit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T, PCMATRIX, const Vector &, const Point &, const Point &)> *>(Context);

    bool Stop = (*ProcessHitRoutine)(ShapeHit->Shape,
                                     ShapeHit->Distance,
                                     ShapeHit->FaceHit,
                                     ShapeHit->AdditionalData,
                                     ShapeHit->AdditionalDataSizeInBytes,
                                     ModelToWorld,
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

RayTracerOwner::RayTracerOwner(
    void
    )
{
    ISTATUS Status = RayTracerOwnerAllocate(&Data);
    
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
RayTracerOwner::TraceClosestHit(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerOwnerTraceSceneProcessClosestHit(Data,
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
RayTracerOwner::TraceClosestHit(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T, PCMATRIX, const Vector &, const Point &, const Point &)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerOwnerTraceSceneProcessClosestHitWithCoordinates(Data,
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
RayTracerOwner::TraceAllHitsOutOfOrder(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerOwnerTraceSceneProcessAllHitsOutOfOrder(Data,
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
RayTracerOwner::TraceAllHitsInOrder(
    _In_ const Scene & Scene,
    _In_ const Ray & WorldRay,
    _In_ std::function<bool(PCSHAPE, FLOAT, INT32, PCVOID, SIZE_T, PCMATRIX, const Vector &, const Point &, const Point &)> ProcessHitRoutine
    )
{
    ISTATUS Status = RayTracerOwnerTraceSceneProcessAllHitsInOrderWithCoordinates(Data,
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

RayTracerOwner::~RayTracerOwner(
    void
    )
{
    RayTracerOwnerFree(Data);
}

} // namespace Iris