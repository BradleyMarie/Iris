/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    ray_tracer_internal.h

Abstract:

    The internal routines for a shape ray tracer.

--*/

#ifndef _IRIS_PHYSX_RAY_TRACER_INTERNAL_
#define _IRIS_PHYSX_RAY_TRACER_INTERNAL_

#include "iris_physx/bsdf_allocator.h"
#include "iris_physx/bsdf_allocator_internal.h"
#include "iris_physx/reflector_compositor.h"
#include "iris_physx/reflector_compositor_internal.h"
#include "iris_physx/texture_coordinate_allocator.h"
#include "iris_physx/texture_coordinate_allocator_internal.h"

//
// Types
//

struct _SHAPE_RAY_TRACER {
    PRAY_TRACER ray_tracer;
    PRAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    float_t minimum_distance;
    REFLECTOR_COMPOSITOR reflector_compositor;
    BSDF_ALLOCATOR bsdf_allocator;
    TEXTURE_COORDINATE_ALLOCATOR texture_coordinate_allocator;
};

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
ShapeRayTracerInitialize(
    _Out_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(shape_ray_tracer != NULL);

    ISTATUS status = RayTracerAllocate(&shape_ray_tracer->ray_tracer);

    if (status != ISTATUS_SUCCESS)
    {
        return false;
    }

    shape_ray_tracer->trace_routine = NULL;
    shape_ray_tracer->trace_context = NULL;
    shape_ray_tracer->minimum_distance = (float_t)0.0;

    bool success = 
        ReflectorCompositorInitialize(&shape_ray_tracer->reflector_compositor);

    if (!success)
    {
        RayTracerFree(shape_ray_tracer->ray_tracer);
        return false;
    }

    BsdfAllocatorInitialize(&shape_ray_tracer->bsdf_allocator);
    TextureCoordinateAllocatorInitialize(
        &shape_ray_tracer->texture_coordinate_allocator);

    return true;
}

static
inline
void
ShapeRayTracerConfigure(
    _Inout_ struct _SHAPE_RAY_TRACER *shape_ray_tracer,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ float_t minimum_distance
    )
{
    assert(shape_ray_tracer != NULL);
    assert(trace_routine != NULL);
    assert(isfinite(minimum_distance) && minimum_distance >= (float_t)0.0);

    shape_ray_tracer->trace_routine = trace_routine;
    shape_ray_tracer->trace_context = trace_context;
    shape_ray_tracer->minimum_distance = minimum_distance;

    ReflectorCompositorClear(&shape_ray_tracer->reflector_compositor);
    BsdfAllocatorClear(&shape_ray_tracer->bsdf_allocator);
    TextureCoordinateAllocatorClear(
        &shape_ray_tracer->texture_coordinate_allocator);
}

static
inline
PREFLECTOR_COMPOSITOR
ShapeRayTracerGetReflectorCompositor(
    _Inout_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(shape_ray_tracer != NULL);

    return &shape_ray_tracer->reflector_compositor;
}

static
inline
void
ShapeRayTracerDestroy(
    _Inout_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(shape_ray_tracer != NULL);

    RayTracerFree(shape_ray_tracer->ray_tracer);
    ReflectorCompositorDestroy(&shape_ray_tracer->reflector_compositor);
    BsdfAllocatorDestroy(&shape_ray_tracer->bsdf_allocator);
    TextureCoordinateAllocatorDestroy(
        &shape_ray_tracer->texture_coordinate_allocator);
}

#endif // _IRIS_PHYSX_RAY_TRACER_INTERNAL_