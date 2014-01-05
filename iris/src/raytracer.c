/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This module implements the RAYTRACER functions.

--*/

#include <irisp.h>

_Check_return_
_Ret_maybenull_
PRAYTRACER
RayTracerAllocate(
    VOID
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PGEOMETRY_HIT_ALLOCATOR GeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PRAYTRACER RayTracer;
    ISTATUS Status;

    RayTracer = (PRAYTRACER) malloc(sizeof(RAYTRACER));

    if (RayTracer == NULL)
    {
        return NULL;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    GeometryHitAllocator = &RayTracer->GeometryHitAllocator;
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

    Status = GeometryHitAllocatorInitialize(GeometryHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        GeometryHitAllocatorDestroy(GeometryHitAllocator);
		IrisConstantPointerListDestroy(PointerList);
        ShapeHitAllocatorDestroy(ShapeHitAllocator);
        free(RayTracer);
        return NULL;
    }

    return RayTracer;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceScene(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCRAY WorldRay,
    _In_ PCSCENE Scene,
    _In_ BOOL SortResults,
    _Outptr_result_buffer_(HitListSize) PCGEOMETRY_HIT **HitList,
    _Out_ PSIZE_T HitListSize
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
	PGEOMETRY_HIT_ALLOCATOR GeometryHitAllocator;
	PIRIS_CONSTANT_POINTER_LIST PointerList;
	PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    ISTATUS Status;

    ASSERT(RayTracer != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(Scene != NULL);
    ASSERT(HitList != NULL);

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    GeometryHitAllocator = &RayTracer->GeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedGeometryHitAllocatorFreeAll(SharedGeometryHitAllocator);
    GeometryHitAllocatorFreeAll(GeometryHitAllocator);
    ShapeHitAllocatorFreeAll(ShapeHitAllocator);
    IrisConstantPointerListClear(PointerList);

    Status = SceneTraceScene(Scene, 
                             WorldRay,
                             ShapeHitAllocator,
                             SharedGeometryHitAllocator,
                             GeometryHitAllocator,
                             PointerList);

	*HitListSize = IrisConstantPointerListGetSize(PointerList);
	*HitList = (PCGEOMETRY_HIT*)IrisConstantPointerListGetStorage(PointerList);

    if (SortResults != FALSE)
    {
		IrisConstantPointerListSort(PointerList, GeometryHitCompare);
    }

    return Status;
}

VOID
RayTracerFree(
    _Pre_maybenull_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PGEOMETRY_HIT_ALLOCATOR GeometryHitAllocator;
    PIRIS_CONSTANT_POINTER_LIST PointerList;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    GeometryHitAllocator = &RayTracer->GeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;
    PointerList = &RayTracer->HitList;

    SharedGeometryHitAllocatorDestroy(SharedGeometryHitAllocator);
    GeometryHitAllocatorDestroy(GeometryHitAllocator);
    ShapeHitAllocatorDestroy(ShapeHitAllocator);
    IrisConstantPointerListDestroy(PointerList);

    free(RayTracer);
}