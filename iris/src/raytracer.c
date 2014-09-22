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
    SHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    IRIS_CONSTANT_POINTER_LIST HitList;
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

    Status = IrisConstantPointerListInitialize(&RayTracer->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        ShapeHitAllocatorDestroy(&RayTracer->ShapeHitAllocator);
        free(RayTracer);
        return NULL;
    }

    Status = SharedGeometryHitAllocatorInitialize(&RayTracer->SharedGeometryHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        IrisConstantPointerListDestroy(&RayTracer->HitList);
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
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedGeometryHitAllocatorFreeAll(SharedGeometryHitAllocator);
    ShapeHitAllocatorFreeAll(ShapeHitAllocator);
    IrisConstantPointerListClear(PointerList);

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
RayTracerTraceGeometry(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCGEOMETRY Geometry
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PINTERNAL_SHAPE_HIT InternalShapeHit;
    PSHAPE_HIT_LIST ShapeHitList;
    ISTATUS Status;

    if (RayTracer == NULL ||
        Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    Status = GeometryTraceGeometry(Geometry,
                                   RayTracer->CurrentRay,
                                   ShapeHitAllocator,
                                   SharedGeometryHitAllocator,
                                   &SharedGeometryHit,
                                   &ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    while (ShapeHitList != NULL)
    {
        InternalShapeHit = (PINTERNAL_SHAPE_HIT) ShapeHitList->ShapeHit;

        InternalShapeHit->SharedGeometryHit = SharedGeometryHit;

        Status = IrisConstantPointerListAddPointer(PointerList,
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
        return ISTATUS_INVALID_ARGUMENT;
    }

    IrisConstantPointerListSort(&RayTracer->HitList,
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
    PCIRIS_CONSTANT_POINTER_LIST PointerList;
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    if (RayTracer == NULL ||
        ShapeHit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    PointerList = &RayTracer->HitList;

    HitCount = IrisConstantPointerListGetSize(PointerList);
    CurrentIndex = RayTracer->HitIndex;

    if (HitCount == CurrentIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = IrisConstantPointerListRetrieveAtIndex(PointerList,
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
    PCIRIS_CONSTANT_POINTER_LIST PointerList;
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PPOINT3 WorldHitPointPointer;
    POINT3 LocalWorldHitPoint;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    if (RayTracer == NULL ||
        ShapeHit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    PointerList = &RayTracer->HitList;

    HitCount = IrisConstantPointerListGetSize(PointerList);
    CurrentIndex = RayTracer->HitIndex;

    if (HitCount == CurrentIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = IrisConstantPointerListRetrieveAtIndex(PointerList,
                                                          CurrentIndex);

    RayTracer->HitIndex = CurrentIndex + 1;

    InternalShapeHit = (PCINTERNAL_SHAPE_HIT) ValueAtIndex;

    SharedGeometryHit = InternalShapeHit->SharedGeometryHit;

    *ShapeHit = &InternalShapeHit->ShapeHit;

    if (ModelToWorld != NULL)
    {
        *ModelToWorld = SharedGeometryHit->ModelToWorld;
    }
    
    if (SharedGeometryHit->Premultiplied != FALSE)
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
                *ModelHitPoint = PointMatrixMultiply(SharedGeometryHit->ModelToWorld,
                                                     *WorldHitPointPointer);
            }
        }
        
        if (ModelViewer != NULL)
        {
            *ModelViewer = VectorMatrixInverseMultiply(SharedGeometryHit->ModelToWorld,
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
                *ModelHitPoint = RayEndpoint(SharedGeometryHit->ModelRay,
                                             InternalShapeHit->ShapeHit.Distance);
            }
        }

        if (ModelViewer != NULL)
        {
            *ModelViewer = SharedGeometryHit->ModelRay.Direction;
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
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedGeometryHitAllocatorDestroy(SharedGeometryHitAllocator);
    ShapeHitAllocatorDestroy(ShapeHitAllocator);
    IrisConstantPointerListDestroy(PointerList);

    free(RayTracer);
}