/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    raytracerowner.c

Abstract:

    This module implements the RAYTRACER_OWNER functions.

--*/

#include <irisp.h>

//
// Types
//

struct _RAYTRACER {
    HIT_ALLOCATOR HitAllocator;
    SHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    CONSTANT_POINTER_LIST HitList;
    SIZE_T HitIndex;
    RAY CurrentRay;
};

struct _RAYTRACER_OWNER {
    RAYTRACER RayTracer;
};

//
// RayTracer Static Functions
//

STATIC
COMPARISON_RESULT
RayTracerInternalHitPointerCompare(
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
RayTracerInitialize(
    _Out_ PRAYTRACER RayTracer
    )
{
    ISTATUS Status;

    ASSERT(RayTracer != NULL);

    Status = HitAllocatorInitialize(&RayTracer->HitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ConstantPointerListInitialize(&RayTracer->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        HitAllocatorDestroy(&RayTracer->HitAllocator);
        return Status;
    }

    Status = SharedHitDataAllocatorInitialize(&RayTracer->SharedHitDataAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        ConstantPointerListDestroy(&RayTracer->HitList);
        HitAllocatorDestroy(&RayTracer->HitAllocator);
        return Status;
    }

    RayTracer->HitIndex = 0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
RayTracerSetRay(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(RayTracer != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    HitAllocator = &RayTracer->HitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorFreeAll(SharedHitDataAllocator);
    HitAllocatorFreeAll(HitAllocator);
    ConstantPointerListClear(PointerList);

    RayTracer->HitIndex = 0;
    RayTracer->CurrentRay = Ray;
}

SFORCEINLINE
VOID
RayTracerSort(
    _Inout_ PRAYTRACER RayTracer
    )
{
    ASSERT(RayTracer != NULL);

    ConstantPointerListSort(&RayTracer->HitList,
                            RayTracerInternalHitPointerCompare);
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerGetNextHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCINTERNAL_HIT *Hit
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    ASSERT(RayTracer != NULL);
    ASSERT(Hit != NULL);

    PointerList = &RayTracer->HitList;

    HitCount = ConstantPointerListGetSize(PointerList);
    CurrentIndex = RayTracer->HitIndex;

    if (HitCount == CurrentIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = ConstantPointerListRetrieveAtIndex(PointerList,
                                                      CurrentIndex);

    RayTracer->HitIndex = CurrentIndex + 1;

    *Hit = (PCINTERNAL_HIT) ValueAtIndex;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
VOID
RayTracerComputeHitData(
    _In_ PCRAYTRACER RayTracer,
    _In_ PCINTERNAL_HIT Hit,
    _Out_ PCMATRIX_REFERENCE *ModelToWorld,
    _Out_ PVECTOR3 ModelViewer,
    _Out_ PPOINT3 ModelHit,
    _Out_ PPOINT3 WorldHit
    )
{
    PCSHARED_HIT_DATA SharedHitData;

    ASSERT(RayTracer != NULL);
    ASSERT(Hit != NULL);
    ASSERT(ModelToWorld != NULL);
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
            *WorldHit = RayEndpoint(RayTracer->CurrentRay,
                                    Hit->Hit.Distance);
        }
        
        *ModelHit = PointMatrixReferenceMultiply(SharedHitData->ModelToWorld,
                                                 *WorldHit);

        *ModelViewer = VectorMatrixReferenceInverseMultiply(SharedHitData->ModelToWorld,
                                                            RayTracer->CurrentRay.Direction);
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

        *WorldHit = RayEndpoint(RayTracer->CurrentRay,
                                Hit->Hit.Distance);
    }
}

SFORCEINLINE
VOID
RayTracerDestroy(
    _In_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(RayTracer != NULL);

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    HitAllocator = &RayTracer->HitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorDestroy(SharedHitDataAllocator);
    HitAllocatorDestroy(HitAllocator);
    ConstantPointerListDestroy(PointerList);
}

//
// RayTracerOwner Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerOwnerAllocate(
    _Out_ PRAYTRACER_OWNER *Result
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    RayTracerOwner = (PRAYTRACER_OWNER) malloc(sizeof(RAYTRACER_OWNER));

    if (RayTracerOwner == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = RayTracerInitialize(&RayTracerOwner->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Result = RayTracerOwner;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
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
    PRAYTRACER RayTracer;
    ISTATUS Status;

    if (RayTracerOwner == NULL)
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
    
    RayTracer = &RayTracerOwner->RayTracer;

    RayTracerSetRay(RayTracer, Ray);

    Status = SceneTrace(Scene, RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerGetNextHit(RayTracer,
                                 &ClosestHit);
    
    if (Status != ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = RayTracerGetNextHit(RayTracer,
                                     &CurrentHit);
        
        if (Status != ISTATUS_NO_MORE_DATA)
        {
            return ISTATUS_SUCCESS;
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
RayTracerOwnerTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT ClosestHit;
    PCINTERNAL_HIT CurrentHit;
    PCMATRIX_REFERENCE ModelToWorld;
    FLOAT ClosestDistance;
    FLOAT CurrentDistance;
    PRAYTRACER RayTracer;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;

    if (RayTracerOwner == NULL)
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

    RayTracer = &RayTracerOwner->RayTracer;
    
    RayTracerSetRay(RayTracer, Ray);

    Status = SceneTrace(Scene, RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerGetNextHit(RayTracer,
                                 &ClosestHit);
    
    if (Status != ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = RayTracerGetNextHit(RayTracer,
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
    
    RayTracerComputeHitData(RayTracer,
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
RayTracerOwnerTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT CurrentHit;
    PRAYTRACER RayTracer;
    ISTATUS Status;

    if (RayTracerOwner == NULL)
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

    RayTracer = &RayTracerOwner->RayTracer;
    
    RayTracerSetRay(RayTracer, Ray);

    Status = SceneTrace(Scene, RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = RayTracerGetNextHit(RayTracer,
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
        
        Status = RayTracerGetNextHit(RayTracer,
                                     &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerOwnerTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCMATRIX_REFERENCE ModelToWorld;
    PCINTERNAL_HIT CurrentHit;
    PRAYTRACER RayTracer;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;

    if (RayTracerOwner == NULL)
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

    RayTracer = &RayTracerOwner->RayTracer;
    
    RayTracerSetRay(RayTracer, Ray);

    Status = SceneTrace(Scene, RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    RayTracerSort(RayTracer);
    
    Status = RayTracerGetNextHit(RayTracer,
                                 &CurrentHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        RayTracerComputeHitData(RayTracer,
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
        
        Status = RayTracerGetNextHit(RayTracer,
                                     &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

IRISAPI
VOID
RayTracerOwnerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER_OWNER RayTracerOwner
    )
{
    if (RayTracerOwner == NULL)
    {
        return;
    }

    RayTracerDestroy(&RayTracerOwner->RayTracer);
    free(RayTracerOwner);
}

//
// RayTracer Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceShape(
    _Inout_ PRAYTRACER RayTracer,
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

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    HitAllocator = &RayTracer->HitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = NULL;
    SharedHitData->Premultiplied = TRUE;
    SharedHitData->ModelRay = RayTracer->CurrentRay;

    Status = ShapeTraceShape(Shape,
                             RayTracer->CurrentRay,
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
RayTracerTraceShapeWithTransform(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCMATRIX_REFERENCE ModelToWorldReference;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;
    RAY TraceRay;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    HitAllocator = &RayTracer->HitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ModelToWorldReference = &ModelToWorld->MatrixReference;
    SharedHitData->ModelToWorld = ModelToWorldReference;

    if (Premultiplied != FALSE)
    {   
        SharedHitData->Premultiplied = TRUE;
        TraceRay = RayTracer->CurrentRay;
    }
    else
    {
        SharedHitData->Premultiplied = FALSE;

        SharedHitData->ModelRay = RayMatrixReferenceInverseMultiply(ModelToWorldReference,
                                                                    RayTracer->CurrentRay);

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
RayTracerTracePremultipliedShapeWithTransform(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCMATRIX_REFERENCE ModelToWorldReference;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    HitAllocator = &RayTracer->HitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    ModelToWorldReference = &ModelToWorld->MatrixReference;
    
    SharedHitData->ModelToWorld = ModelToWorldReference;
    SharedHitData->Premultiplied = TRUE;

    Status = ShapeTraceShape(Shape,
                             RayTracer->CurrentRay,
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
RayTracerGetRay(
    _In_ PRAYTRACER RayTracer,
    _Out_ PRAY Ray
    )
{
    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Ray == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Ray = RayTracer->CurrentRay;

    return ISTATUS_SUCCESS;
}
