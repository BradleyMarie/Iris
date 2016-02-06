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
    SHAPE_HIT_ALLOCATOR ShapeHitAllocator;
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
RayTracerInternalShapeHitPointerCompare(
    _In_ PCVOID Hit0,
    _In_ PCVOID Hit1
    )
{
    PCINTERNAL_SHAPE_HIT *InternalShapeHit0;
    PCINTERNAL_SHAPE_HIT *InternalShapeHit1;
    COMPARISON_RESULT Result;

    ASSERT(Hit0 != NULL);
    ASSERT(Hit1 != NULL);

    InternalShapeHit0 = (PCINTERNAL_SHAPE_HIT*) Hit0;
    InternalShapeHit1 = (PCINTERNAL_SHAPE_HIT*) Hit1;

    Result = InternalShapeHitCompare(*InternalShapeHit0, *InternalShapeHit1);

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

    Status = ShapeHitAllocatorInitialize(&RayTracer->ShapeHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ConstantPointerListInitialize(&RayTracer->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        ShapeHitAllocatorDestroy(&RayTracer->ShapeHitAllocator);
        return Status;
    }

    Status = SharedHitDataAllocatorInitialize(&RayTracer->SharedHitDataAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        ConstantPointerListDestroy(&RayTracer->HitList);
        ShapeHitAllocatorDestroy(&RayTracer->ShapeHitAllocator);
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
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    ASSERT(RayTracer != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorFreeAll(SharedHitDataAllocator);
    ShapeHitAllocatorFreeAll(ShapeHitAllocator);
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
                            RayTracerInternalShapeHitPointerCompare);
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerGetNextInternalHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCINTERNAL_SHAPE_HIT *ShapeHit
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    ASSERT(RayTracer != NULL);
    ASSERT(ShapeHit != NULL);

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

    *ShapeHit = (PCINTERNAL_SHAPE_HIT) ValueAtIndex;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
VOID
RayTracerComputeHitData(
    _In_ PCRAYTRACER RayTracer,
    _In_ PCINTERNAL_SHAPE_HIT InternalShapeHit,
    _Out_ PCSHAPE_HIT *ShapeHit,
    _Out_ PCMATRIX *ModelToWorld,
    _Out_ PVECTOR3 ModelViewer,
    _Out_ PPOINT3 ModelHit,
    _Out_ PPOINT3 WorldHit
    )
{
    PCSHARED_HIT_DATA SharedHitData;

    ASSERT(InternalShapeHit != NULL);
    ASSERT(ShapeHit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(WorldHit != NULL);

    SharedHitData = InternalShapeHit->SharedHitData;

    *ShapeHit = &InternalShapeHit->ShapeHit;
    *ModelToWorld = SharedHitData->ModelToWorld;
    
    if (SharedHitData->Premultiplied != FALSE)
    {
        if (InternalShapeHit->ModelHitPointValid != FALSE)
        {
            *WorldHit = InternalShapeHit->ModelHitPoint;
        }
        else
        {
            *WorldHit = RayEndpoint(RayTracer->CurrentRay,
                                    InternalShapeHit->ShapeHit.Distance);
        }
        
        *ModelHit = PointMatrixMultiply(SharedHitData->ModelToWorld,
                                        *WorldHit);

        *ModelViewer = VectorMatrixInverseMultiply(SharedHitData->ModelToWorld,
                                                   RayTracer->CurrentRay.Direction);
    }
    else
    {
        if (InternalShapeHit->ModelHitPointValid != FALSE)
        {
            *ModelHit = InternalShapeHit->ModelHitPoint;
        }
        else
        {
            *ModelHit = RayEndpoint(SharedHitData->ModelRay,
                                    InternalShapeHit->ShapeHit.Distance);
        }

        *ModelViewer = SharedHitData->ModelRay.Direction;

        *WorldHit = RayEndpoint(RayTracer->CurrentRay,
                                InternalShapeHit->ShapeHit.Distance);
    }
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerGetNextShapeHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCSHAPE_HIT *ShapeHit
    )
{
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    ISTATUS Status;
 
    ASSERT(RayTracer != NULL);
    ASSERT(ShapeHit != NULL);
    
    Status = RayTracerGetNextInternalHit(RayTracer,
                                         &InternalShapeHit);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *ShapeHit = &InternalShapeHit->ShapeHit;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerGetNextHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCSHAPE_HIT *ShapeHit,
    _Out_ PCMATRIX *ModelToWorld,
    _Out_ PVECTOR3 ModelViewer,
    _Out_ PPOINT3 ModelHit,
    _Out_ PPOINT3 WorldHit
    )
{
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    ISTATUS Status;
 
    ASSERT(RayTracer != NULL);
    ASSERT(ShapeHit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(WorldHit != NULL);
    
    Status = RayTracerGetNextInternalHit(RayTracer,
                                         &InternalShapeHit);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    RayTracerComputeHitData(RayTracer,
                            InternalShapeHit,
                            ShapeHit,
                            ModelToWorld,
                            ModelViewer,
                            ModelHit,
                            WorldHit);
                                     
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
RayTracerDestroy(
    _In_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    ASSERT(RayTracer != NULL);

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorDestroy(SharedHitDataAllocator);
    ShapeHitAllocatorDestroy(ShapeHitAllocator);
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
RayTracerOwnerTraceSceneFindClosestHit(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PCSHAPE_HIT ClosestHit;
    PCSHAPE_HIT ShapeHit;
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
    
    Status = RayTracerGetNextShapeHit(RayTracer,
                                      &ClosestHit);
    
    if (Status != ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    while (TRUE)
    {
        Status = RayTracerGetNextShapeHit(RayTracer,
                                          &ShapeHit);
                                          
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (ShapeHit->Distance > MinimumDistance &&
            ShapeHit->Distance < ClosestHit->Distance)
        {
            ClosestHit = ShapeHit;
        }
    }
    
    Status = ProcessHitRoutine(ProcessHitRoutineContext, 
                               ShapeHit);
    
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
RayTracerOwnerTraceSceneFindClosestHitWithData(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PCINTERNAL_SHAPE_HIT ClosestHit;
    PCMATRIX ModelToWorld;
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
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
    
    Status = RayTracerGetNextInternalHit(RayTracer,
                                         &ClosestHit);
    
    if (Status != ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    while (TRUE)
    {
        Status = RayTracerGetNextInternalHit(RayTracer,
                                             &InternalShapeHit);
                                          
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (InternalShapeHit->ShapeHit.Distance > MinimumDistance &&
            InternalShapeHit->ShapeHit.Distance < ClosestHit->ShapeHit.Distance)
        {
            ClosestHit = InternalShapeHit;
        }
    }
    
    RayTracerComputeHitData(RayTracer,
                            ClosestHit,
                            &ShapeHit,
                            &ModelToWorld,
                            &ModelViewer,
                            &ModelHit,
                            &WorldHit);
    
    Status = ProcessHitRoutine(ProcessHitRoutineContext, 
                               ShapeHit,
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
RayTracerOwnerTraceSceneFindAllHitsUnsorted(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PCSHAPE_HIT ShapeHit;
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
    
    Status = RayTracerGetNextShapeHit(RayTracer,
                                      &ShapeHit);
    
    while (Status != ISTATUS_NO_MORE_DATA)
    {
        Status = ProcessHitRoutine(ProcessHitRoutineContext, 
                                   ShapeHit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Status = RayTracerGetNextShapeHit(RayTracer,
                                          &ShapeHit);
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerOwnerTraceSceneFindAllHits(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PCMATRIX ModelToWorld;
    PCSHAPE_HIT ShapeHit;
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
                                 &ShapeHit,
                                 &ModelToWorld,
                                 &ModelViewer,
                                 &ModelHit,
                                 &WorldHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        Status = ProcessHitRoutine(ProcessHitRoutineContext, 
                                   ShapeHit,
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
                                     &ShapeHit,
                                     &ModelToWorld,
                                     &ModelViewer,
                                     &ModelHit,
                                     &WorldHit);
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
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
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
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
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
                             ShapeHitAllocator,
                             &ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (ShapeHitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (ShapeHitList != NULL)
    {
        InternalShapeHit = (PINTERNAL_SHAPE_HIT) ShapeHitList->ShapeHit;

        InternalShapeHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalShapeHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        ShapeHitList = ShapeHitList->NextShapeHit;
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
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
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
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = ModelToWorld;

    if (Premultiplied != FALSE)
    {   
        SharedHitData->Premultiplied = TRUE;
        TraceRay = RayTracer->CurrentRay;
    }
    else
    {
        SharedHitData->Premultiplied = FALSE;

        SharedHitData->ModelRay = RayMatrixInverseMultiply(ModelToWorld,
                                                           RayTracer->CurrentRay);

        TraceRay = SharedHitData->ModelRay;
    }

    Status = ShapeTraceShape(Shape,
                             TraceRay,
                             ShapeHitAllocator,
                             &ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (ShapeHitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (ShapeHitList != NULL)
    {
        InternalShapeHit = (PINTERNAL_SHAPE_HIT) ShapeHitList->ShapeHit;

        InternalShapeHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalShapeHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        ShapeHitList = ShapeHitList->NextShapeHit;
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
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
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
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = ModelToWorld;
    SharedHitData->Premultiplied = TRUE;

    Status = ShapeTraceShape(Shape,
                             RayTracer->CurrentRay,
                             ShapeHitAllocator,
                             &ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (ShapeHitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (ShapeHitList != NULL)
    {
        InternalShapeHit = (PINTERNAL_SHAPE_HIT) ShapeHitList->ShapeHit;

        InternalShapeHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalShapeHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        ShapeHitList = ShapeHitList->NextShapeHit;
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
