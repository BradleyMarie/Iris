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
static
ISTATUS 
ProcessHitsAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T)> *>(Context);
    
    bool Stop = (*ProcessHitRoutine)(Hit->ShapeReference,
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
    _In_ PCMATRIX_REFERENCE ModelToWorldReference,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    auto ProcessHitRoutine = static_cast<std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> *>(Context);

    bool Stop = (*ProcessHitRoutine)(Hit->ShapeReference,
                                     Hit->Distance,
                                     Hit->FaceHit,
                                     Hit->AdditionalData,
                                     Hit->AdditionalDataSizeInBytes,
                                     MatrixReference(ModelToWorldReference),
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
    _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
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
    _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
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
    _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T)> ProcessHitRoutine
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
    _In_ std::function<bool(PCSHAPE_REFERENCE, FLOAT, INT32, PCVOID, SIZE_T, MatrixReference, Vector, Point, Point)> ProcessHitRoutine
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