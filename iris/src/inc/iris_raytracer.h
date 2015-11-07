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
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerInitialize(
    _Out_ PRAYTRACER Result,
    _In_ RAY Ray
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(Result != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

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

    RayTracer->CurrentRay = Ray;
    RayTracer->HitIndex = 0;

    *Result = RayTracer;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
VOID
RayTracerSetRay(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ BOOL NormalizeRay
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

    if (NormalizeRay != FALSE)
    {
        RayTracer->CurrentRay = RayNormalize(Ray);
    }
    else
    {
        RayTracer->CurrentRay = Ray;
    }
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
VOID
RayTracerGetRay(
    _In_ PRAYTRACER RayTracer,
    _Out_ PRAY Ray
    )
{
    ASSERT(RayTracer != NULL);
    ASSERT(Ray != NULL);

    *Ray = RayTracer->CurrentRay;
}

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

#endif // _RAYTRACER_OWNER_IRIS_