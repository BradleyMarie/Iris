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
_Ret_maybenull_
PRAYTRACER
RayTracerAllocate(
    _In_ RAY Ray
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
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

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    Status = ShapeHitAllocatorInitialize(ShapeHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        free(RayTracer);
        return NULL;
    }

    Status = IrisConstantPointerListInitialize(PointerList);

    if (Status != ISTATUS_SUCCESS)
    {
        ShapeHitAllocatorDestroy(ShapeHitAllocator);
        free(RayTracer);
        return NULL;
    }

    Status = SharedGeometryHitAllocatorInitialize(SharedGeometryHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        IrisConstantPointerListDestroy(PointerList);
        ShapeHitAllocatorDestroy(ShapeHitAllocator);
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
    _In_ RAY Ray
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL ||
        RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedGeometryHitAllocatorFreeAll(SharedGeometryHitAllocator);
    ShapeHitAllocatorFreeAll(ShapeHitAllocator);
    IrisConstantPointerListClear(PointerList);

    RayTracer->CurrentRay = Ray;
    RayTracer->HitIndex = 0;

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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
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
RayTracerGetNextGeometryHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PGEOMETRY_HIT GeometryHit
    )
{
    PCIRIS_CONSTANT_POINTER_LIST PointerList;
    PCINTERNAL_SHAPE_HIT InternalShapeHit;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    if (RayTracer == NULL ||
        GeometryHit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    PointerList = &RayTracer->HitList;

    HitCount = IrisConstantPointerListGetSize(PointerList);
    CurrentIndex = RayTracer->HitIndex;

    if (HitCount == RayTracer->HitIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = IrisConstantPointerListRetrieveAtIndex(PointerList,
                                                          CurrentIndex);

    RayTracer->HitIndex = CurrentIndex + 1;

    InternalShapeHit = (PCINTERNAL_SHAPE_HIT) ValueAtIndex;

    GeometryHitInitialize(GeometryHit,
                          RayTracer->CurrentRay,
                          InternalShapeHit);

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

VOID
RayTracerFree(
    _Pre_maybenull_ _Post_invalid_ PRAYTRACER RayTracer
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