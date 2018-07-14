/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray_tracer_internal.h

Abstract:

    The internal routines for a shape ray tracer.

--*/

#ifndef _IRIS_PHYSX_RAY_TRACER_INTERNAL_
#define _IRIS_PHYSX_RAY_TRACER_INTERNAL_

#include "iris_physx/brdf_allocator.h"

//
// Types
//

struct _SHAPE_RAY_TRACER {
    PRAY_TRACER ray_tracer;
    float_t minimum_distance;
    PRAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    PBRDF_ALLOCATOR brdf_allocator;
};

//
// Functions
//

static
inline
void
ShapeRayTracerInitialize(
    _In_ PRAY_TRACER ray_tracer,
    _In_ float_t minimum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PBRDF_ALLOCATOR brdf_allocator,
    _Out_ struct _SHAPE_RAY_TRACER *shape_ray_tracer
    )
{
    assert(ray_tracer != NULL);
    assert(isfinite(minimum_distance) && minimum_distance >= (float_t)0.0);
    assert(trace_routine != NULL);
    assert(brdf_allocator != NULL);
    assert(shape_ray_tracer != NULL);

    shape_ray_tracer->ray_tracer = ray_tracer;
    shape_ray_tracer->minimum_distance = minimum_distance;
    shape_ray_tracer->trace_routine = trace_routine;
    shape_ray_tracer->trace_context = trace_context;
    shape_ray_tracer->brdf_allocator = brdf_allocator;
}

#endif // _IRIS_PHYSX_RAY_TRACER_INTERNAL_