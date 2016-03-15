/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This module implements the raytracer functions.

--*/

#include <irisp.h>

//
// Types
//

struct _RAYTRACER_REFERENCE {
    HIT_ALLOCATOR HitAllocator;
    SHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    CONSTANT_POINTER_LIST HitList;
    SIZE_T HitIndex;
    RAY CurrentRay;
};

struct _RAYTRACER {
    RAYTRACER_REFERENCE RayTracerReference;
};

//
// RayTracerReference Static Functions
//

STATIC
COMPARISON_RESULT
RayTracerReferenceInternalHitPointerCompare(
    _In_ PCVOID Hit0,
    _In_ PCVOID Hit1
    )
{
    PCINTERNAL_HIT *InternalHit0;
    PCINTERNAL_HIT *InternalHit1;
    COMPARISON_RESULT Result;

    ASSERT(Hit0 != NULL);
    ASSERT(Hit1 != NULL);

    InternalHit0 = (PCINTERNAL_HIT*) Hit0;
    InternalHit1 = (PCINTERNAL_HIT*) Hit1;

    Result = InternalHitCompare(*InternalHit0, *InternalHit1);

    return Result;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerReferenceInitialize(
    _Out_ PRAYTRACER_REFERENCE RayTracerReference
    )
{
    ISTATUS Status;

    ASSERT(RayTracerReference != NULL);

    Status = HitAllocatorInitialize(&RayTracerReference->HitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ConstantPointerListInitialize(&RayTracerReference->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        HitAllocatorDestroy(&RayTracerReference->HitAllocator);
        return Status;
    }

    Status = SharedHitDataAllocatorInitialize(&RayTracerReference->SharedHitDataAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        ConstantPointerListDestroy(&RayTracerReference->HitList);
        HitAllocatorDestroy(&RayTracerReference->HitAllocator);
        return Status;
    }

    RayTracerReference->HitIndex = 0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
RayTracerReferenceSetRay(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ RAY Ray
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(RayTracerReference != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    SharedHitDataAllocator = &RayTracerReference->SharedHitDataAllocator;
    HitAllocator = &RayTracerReference->HitAllocator;
    PointerList = &RayTracerReference->HitList;

    SharedHitDataAllocatorFreeAll(SharedHitDataAllocator);
    HitAllocatorFreeAll(HitAllocator);
    ConstantPointerListClear(PointerList);

    RayTracerReference->HitIndex = 0;
    RayTracerReference->CurrentRay = Ray;
}

SFORCEINLINE
VOID
RayTracerReferenceSort(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference
    )
{
    ASSERT(RayTracerReference != NULL);

    ConstantPointerListSort(&RayTracerReference->HitList,
                            RayTracerReferenceInternalHitPointerCompare);
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerReferenceGetNextHit(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _Out_ PCINTERNAL_HIT *Hit
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    ASSERT(RayTracerReference != NULL);
    ASSERT(Hit != NULL);

    PointerList = &RayTracerReference->HitList;

    HitCount = ConstantPointerListGetSize(PointerList);
    CurrentIndex = RayTracerReference->HitIndex;

    if (HitCount == CurrentIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = ConstantPointerListRetrieveAtIndex(PointerList,
                                                      CurrentIndex);

    RayTracerReference->HitIndex = CurrentIndex + 1;

    *Hit = (PCINTERNAL_HIT) ValueAtIndex;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
VOID
RayTracerReferenceComputeHitData(
    _In_ PCRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCINTERNAL_HIT Hit,
    _Out_ PCMATRIX *ModelToWorld,
    _Out_ PVECTOR3 ModelViewer,
    _Out_ PPOINT3 ModelHit,
    _Out_ PPOINT3 WorldHit
    )
{
    PCSHARED_HIT_DATA SharedHitData;

    ASSERT(RayTracerReference != NULL);
    ASSERT(Hit != NULL);
    ASSERT(ModelToWorldReference != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(WorldHit != NULL);

    SharedHitData = Hit->SharedHitData;

    *ModelToWorld = SharedHitData->ModelToWorld;
    
    if (SharedHitData->Premultiplied != FALSE)
    {
        if (Hit->ModelHitPointValid != FALSE)
        {
            *WorldHit = Hit->ModelHitPoint;
        }
        else
        {
            *WorldHit = RayEndpoint(RayTracerReference->CurrentRay,
                                    Hit->Hit.Distance);
        }
        
        *ModelHit = PointMatrixMultiply(SharedHitData->ModelToWorld,
                                        *WorldHit);

        *ModelViewer = VectorMatrixInverseMultiply(SharedHitData->ModelToWorld,
                                                   RayTracerReference->CurrentRay.Direction);
    }
    else
    {
        if (Hit->ModelHitPointValid != FALSE)
        {
            *ModelHit = Hit->ModelHitPoint;
        }
        else
        {
            *ModelHit = RayEndpoint(SharedHitData->ModelRay,
                                    Hit->Hit.Distance);
        }

        *ModelViewer = SharedHitData->ModelRay.Direction;

        *WorldHit = RayEndpoint(RayTracerReference->CurrentRay,
                                Hit->Hit.Distance);
    }
}

SFORCEINLINE
VOID
RayTracerReferenceDestroy(
    _In_ _Post_invalid_ PRAYTRACER_REFERENCE RayTracerReference
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(RayTracerReference != NULL);

    SharedHitDataAllocator = &RayTracerReference->SharedHitDataAllocator;
    HitAllocator = &RayTracerReference->HitAllocator;
    PointerList = &RayTracerReference->HitList;

    SharedHitDataAllocatorDestroy(SharedHitDataAllocator);
    HitAllocatorDestroy(HitAllocator);
    ConstantPointerListDestroy(PointerList);
}

//
// RayTracer Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerAllocate(
    _Out_ PRAYTRACER *Result
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    RayTracer = (PRAYTRACER) malloc(sizeof(RAYTRACER));

    if (RayTracer == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = RayTracerReferenceInitialize(&RayTracer->RayTracerReference);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Result = RayTracer;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT CurrentHit;
    PCINTERNAL_HIT ClosestHit;
    FLOAT ClosestDistance;
    FLOAT CurrentDistance;
    PRAYTRACER_REFERENCE RayTracerReference;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (IsLessThanZeroFloat(MinimumDistance) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    RayTracerReference = &RayTracer->RayTracerReference;

    RayTracerReferenceSetRay(RayTracerReference, Ray);

    Status = SceneTrace(Scene, RayTracerReference);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                          &ClosestHit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                              &CurrentHit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        CurrentDistance = ClosestHit->Hit.Distance;
        
        if (CurrentDistance > MinimumDistance &&
            CurrentDistance < ClosestDistance)
        {
            ClosestDistance = CurrentDistance;
            ClosestHit = CurrentHit;
        }
    }
    
    Status = ProcessHitRoutine(ProcessHitContext,
                               &ClosestHit->Hit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT ClosestHit;
    PCINTERNAL_HIT CurrentHit;
    PCMATRIX ModelToWorld;
    FLOAT ClosestDistance;
    FLOAT CurrentDistance;
    PRAYTRACER_REFERENCE RayTracerReference;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (IsLessThanZeroFloat(MinimumDistance) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    RayTracerReference = &RayTracer->RayTracerReference;
    
    RayTracerReferenceSetRay(RayTracerReference, Ray);

    Status = SceneTrace(Scene, RayTracerReference);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                          &ClosestHit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                              &CurrentHit);
                                          
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        CurrentDistance = CurrentHit->Hit.Distance;
        
        if (CurrentDistance > MinimumDistance &&
            CurrentDistance < ClosestDistance)
        {
            ClosestDistance = CurrentDistance;
            ClosestHit = CurrentHit;
        }
    }
    
    RayTracerReferenceComputeHitData(RayTracerReference,
                                     ClosestHit,
                                     &ModelToWorld,
                                     &ModelViewer,
                                     &ModelHit,
                                     &WorldHit);
    
    Status = ProcessHitRoutine(ProcessHitContext,
                               &CurrentHit->Hit,
                               ModelToWorld,
                               ModelViewer,
                               ModelHit,
                               WorldHit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT CurrentHit;
    PRAYTRACER_REFERENCE RayTracerReference;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    RayTracerReference = &RayTracer->RayTracerReference;
    
    RayTracerReferenceSetRay(RayTracerReference, Ray);

    Status = SceneTrace(Scene, RayTracerReference);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                          &CurrentHit);
    
    while (Status != ISTATUS_NO_MORE_DATA)
    {                            
        Status = ProcessHitRoutine(ProcessHitContext,
                                   &CurrentHit->Hit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                              &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCMATRIX ModelToWorld;
    PCINTERNAL_HIT CurrentHit;
    PRAYTRACER_REFERENCE RayTracerReference;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    RayTracerReference = &RayTracer->RayTracerReference;
    
    RayTracerReferenceSetRay(RayTracerReference, Ray);

    Status = SceneTrace(Scene, RayTracerReference);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    RayTracerReferenceSort(RayTracerReference);
    
    Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                          &CurrentHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        RayTracerReferenceComputeHitData(RayTracerReference,
                                         CurrentHit,
                                         &ModelToWorld,
                                         &ModelViewer,
                                         &ModelHit,
                                         &WorldHit);

        Status = ProcessHitRoutine(ProcessHitContext,
                                   &CurrentHit->Hit,
                                   ModelToWorld,
                                   ModelViewer,
                                   ModelHit,
                                   WorldHit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Status = RayTracerReferenceGetNextHit(RayTracerReference,
                                              &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

IRISAPI
VOID
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    if (RayTracer == NULL)
    {
        return;
    }

    RayTracerReferenceDestroy(&RayTracer->RayTracerReference);
    free(RayTracer);
}

//
// RayTracerReference Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerReferenceTraceShape(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;

    if (RayTracerReference == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracerReference->SharedHitDataAllocator;
    HitAllocator = &RayTracerReference->HitAllocator;
    PointerList = &RayTracerReference->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = NULL;
    SharedHitData->Premultiplied = TRUE;
    SharedHitData->ModelRay = RayTracerReference->CurrentRay;

    Status = ShapeTraceShape(Shape,
                             RayTracerReference->CurrentRay,
                             HitAllocator,
                             &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerReferenceTraceShapeWithTransform(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;
    RAY TraceRay;

    if (RayTracerReference == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracerReference->SharedHitDataAllocator;
    HitAllocator = &RayTracerReference->HitAllocator;
    PointerList = &RayTracerReference->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = ModelToWorld;

    if (Premultiplied != FALSE)
    {   
        SharedHitData->Premultiplied = TRUE;
        TraceRay = RayTracerReference->CurrentRay;
    }
    else
    {
        SharedHitData->Premultiplied = FALSE;

        SharedHitData->ModelRay = RayMatrixInverseMultiply(ModelToWorld,
                                                           RayTracerReference->CurrentRay);

        TraceRay = SharedHitData->ModelRay;
    }

    Status = ShapeTraceShape(Shape,
                             TraceRay,
                             HitAllocator,
                             &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerReferenceTracePremultipliedShapeWithTransform(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;

    if (RayTracerReference == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracerReference->SharedHitDataAllocator;
    HitAllocator = &RayTracerReference->HitAllocator;
    PointerList = &RayTracerReference->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    SharedHitData->ModelToWorld = ModelToWorld;
    SharedHitData->Premultiplied = TRUE;

    Status = ShapeTraceShape(Shape,
                             RayTracerReference->CurrentRay,
                             HitAllocator,
                             &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerReferenceGetRay(
    _In_ PRAYTRACER_REFERENCE RayTracerReference,
    _Out_ PRAY Ray
    )
{
    if (RayTracerReference == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Ray == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Ray = RayTracerReference->CurrentRay;

    return ISTATUS_SUCCESS;
}
