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
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    PRAYTRACER RayTracer;
    ISTATUS Status;

    RayTracer = (PRAYTRACER) malloc(sizeof(RAYTRACER));

    if (RayTracer == NULL)
    {
        return NULL;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;

    Status = ShapeHitAllocatorInitialize(ShapeHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        free(RayTracer);
        return NULL;
    }

    Status = SharedGeometryHitAllocatorInitialize(SharedGeometryHitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        free(RayTracer);
        ShapeHitAllocatorDestroy(ShapeHitAllocator);
        return NULL;
    }

    return RayTracer;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceScene(
    _In_ PRAYTRACER RayTracer,
    _In_ PRAY WorldRay,
    _In_ PSCENE Scene,
    _In_ BOOL SortResults,
    _Outptr_result_maybenull_ PSHAPE_HIT *HitList 
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    ISTATUS Status;

    ASSERT(RayTracer != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(Scene != NULL);
    ASSERT(HitList != NULL);

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;

    Status = SceneTraceScene(Scene, 
                             WorldRay,
							 ShapeHitAllocator,
                             SharedGeometryHitAllocator,
                             HitList);

    //
    // TODO: Sort Hits
    //

    return Status;
}

VOID
RayTracerFree(
    _Pre_maybenull_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    PSHAPE_HIT_ALLOCATOR ShapeHitAllocator;

    if (RayTracer == NULL)
    {
        return;
    }

    SharedGeometryHitAllocator = &RayTracer->SharedGeometryHitAllocator;
    ShapeHitAllocator = &RayTracer->ShapeHitAllocator;

    ShapeHitAllocatorDestroy(ShapeHitAllocator);
    SharedGeometryHitAllocatorDestroy(SharedGeometryHitAllocator);

    free(RayTracer);
}