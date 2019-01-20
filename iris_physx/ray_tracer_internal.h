/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    ray_tracer_internal.h

Abstract:

    The internal routines for a shape ray tracer.

--*/

#ifndef _IRIS_PHYSX_RAY_TRACER_INTERNAL_
#define _IRIS_PHYSX_RAY_TRACER_INTERNAL_

#include "iris_physx/brdf_allocator.h"
#include "iris_physx/brdf_allocator_internal.h"
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/reflector_allocator_internal.h"

//
// Types
//

struct _SHAPE_RAY_TRACER {
    PRAY_TRACER ray_tracer;
    PRAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    float_t minimum_distance;
    REFLECTOR_ALLOCATOR reflector_allocator;
    BRDF_ALLOCATOR brdf_allocator;
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

    ReflectorAllocatorInitialize(&shape_ray_tracer->reflector_allocator);
    BrdfAllocatorInitialize(&shape_ray_tracer->brdf_allocator);

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
}

static
inline
PREFLECTOR_ALLOCATOR
ShapeRayTracerGetReflectorAllocator(
    _Inout_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(shape_ray_tracer != NULL);

    return &shape_ray_tracer->reflector_allocator;
}

static
inline
void
ShapeRayTracerClear(
    _Inout_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(shape_ray_tracer != NULL);

    ReflectorAllocatorClear(&shape_ray_tracer->reflector_allocator);
    BrdfAllocatorClear(&shape_ray_tracer->brdf_allocator);
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
    ReflectorAllocatorDestroy(&shape_ray_tracer->reflector_allocator);
    BrdfAllocatorDestroy(&shape_ray_tracer->brdf_allocator);
}

#endif // _IRIS_PHYSX_RAY_TRACER_INTERNAL_