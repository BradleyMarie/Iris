/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This module implements the RAYTRACER functions.

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

//
// Static Functions
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

//
// Public Functions
//

_Check_return_
_Ret_opt_
PRAYTRACER
RayTracerAllocate(
    _In_ RAY Ray
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    if (RayValidate(Ray) == FALSE)
    {
        return NULL;
    }

    RayTracer = (PRAYTRACER) malloc(sizeof(RAYTRACER));

    if (RayTracer == NULL)
    {
        return NULL;
    }

    Status = ShapeHitAllocatorInitialize(&RayTracer->ShapeHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        free(RayTracer);
        return NULL;
    }

    Status = ConstantPointerListInitialize(&RayTracer->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        ShapeHitAllocatorDestroy(&RayTracer->ShapeHitAllocator);
        free(RayTracer);
        return NULL;
    }

    Status = SharedHitDataAllocatorInitialize(&RayTracer->SharedHitDataAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        ConstantPointerListDestroy(&RayTracer->HitList);
        ShapeHitAllocatorDestroy(&RayTracer->ShapeHitAllocator);
        free(RayTracer);
        return NULL;
    }

    RayTracer->CurrentRay = Ray;
    RayTracer->HitIndex = 0;

    return RayTracer;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerSetRay(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ BOOL NormalizeRay
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorFreeAll(SharedHitDataAllocator);
    ShapeHitAllocatorFreeAll(ShapeHitAllocator);
    ConstantPointerListClear(PointerList);

    RayTracer->HitIndex = 0;

    if (NormalizeRay != FALSE)
    {
        RayTracer->CurrentRay = RayNormalize(Ray);
    }
    else
    {
        RayTracer->CurrentRay = Ray;
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

ISTATUS
RayTracerSort(
    _Inout_ PRAYTRACER RayTracer
    )
{
    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ConstantPointerListSort(&RayTracer->HitList,
                            RayTracerInternalShapeHitPointerCompare);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerGetNextShapeHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCSHAPE_HIT *ShapeHit
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (ShapeHit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

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

    InternalShapeHit = (PCINTERNAL_SHAPE_HIT) ValueAtIndex;

    *ShapeHit = &InternalShapeHit->ShapeHit;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerGetNextHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCSHAPE_HIT *ShapeHit,
    _Out_opt_ PVECTOR3 ModelViewer,
    _Out_opt_ PPOINT3 ModelHitPoint,
    _Out_opt_ PPOINT3 WorldHitPoint,
    _Out_opt_ PCMATRIX *ModelToWorld
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCSHARED_HIT_DATA SharedHitData;
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PPOINT3 WorldHitPointPointer;
    POINT3 LocalWorldHitPoint;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (ShapeHit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

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

    InternalShapeHit = (PCINTERNAL_SHAPE_HIT) ValueAtIndex;

    SharedHitData = InternalShapeHit->SharedHitData;

    *ShapeHit = &InternalShapeHit->ShapeHit;

    if (ModelToWorld != NULL)
    {
        *ModelToWorld = SharedHitData->ModelToWorld;
    }
    
    if (SharedHitData->Premultiplied != FALSE)
    {
        if (ModelHitPoint != NULL ||
            WorldHitPoint != NULL)
        {
            if (ModelHitPoint == NULL)
            {
                WorldHitPointPointer = &LocalWorldHitPoint;
            }
            else
            {
                WorldHitPointPointer = ModelHitPoint;
            }

            if (InternalShapeHit->ModelHitPointValid != FALSE)
            {
                *WorldHitPointPointer = InternalShapeHit->ModelHitPoint;
            }
            else
            {
                *WorldHitPointPointer = RayEndpoint(RayTracer->CurrentRay,
                                                    InternalShapeHit->ShapeHit.Distance);
            }

            if (WorldHitPoint != NULL)
            {
                *WorldHitPoint = *WorldHitPointPointer;
            }

            if (ModelHitPoint != NULL)
            {
                *ModelHitPoint = PointMatrixMultiply(SharedHitData->ModelToWorld,
                                                     *WorldHitPointPointer);
            }
        }
        
        if (ModelViewer != NULL)
        {
            *ModelViewer = VectorMatrixInverseMultiply(SharedHitData->ModelToWorld,
                                                       RayTracer->CurrentRay.Direction);
        }
    }
    else
    {
        if (ModelHitPoint != NULL)
        {
            if (InternalShapeHit->ModelHitPointValid != FALSE)
            {
                *ModelHitPoint = InternalShapeHit->ModelHitPoint;
            }
            else
            {
                *ModelHitPoint = RayEndpoint(SharedHitData->ModelRay,
                                             InternalShapeHit->ShapeHit.Distance);
            }
        }

        if (ModelViewer != NULL)
        {
            *ModelViewer = SharedHitData->ModelRay.Direction;
        }

        if (WorldHitPoint != NULL)
        {
            *WorldHitPoint = RayEndpoint(RayTracer->CurrentRay, 
                                         InternalShapeHit->ShapeHit.Distance);
        }
    }

    return ISTATUS_SUCCESS;
}

VOID
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return;
    }

    SharedHitDataAllocator = &RayTracer->SharedHitDataAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedHitDataAllocatorDestroy(SharedHitDataAllocator);
    ShapeHitAllocatorDestroy(ShapeHitAllocator);
    ConstantPointerListDestroy(PointerList);

    free(RayTracer);
}