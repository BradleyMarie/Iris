/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    ray_tracer.h

Abstract:

    The top level ray tracer type used by Iris. Manages lifetimes of hit tester,
    hit allocator, and any allocated hits. 

--*/

#ifndef _IRIS_RAY_TRACER_
#define _IRIS_RAY_TRACER_

#include "iris/hit_context.h"
#include "iris/hit_tester.h"
#include "iris/ray.h"

//
// Types
//

typedef
ISTATUS 
(*PRAY_TRACER_TRACE_ROUTINE)(
    _In_opt_ const void *context, 
    _Inout_ PHIT_TESTER hit_tester,
    _In_ RAY ray
    );

typedef
ISTATUS 
(*PRAY_TRACER_PROCESS_HIT_ROUTINE)(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context
    );
    
typedef
ISTATUS 
(*PRAY_TRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE)(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context,
    _In_ PCMATRIX model_to_world,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point
    );

typedef struct _RAY_TRACER RAY_TRACER, *PRAY_TRACER;
typedef const RAY_TRACER *PCRAY_TRACER;

//
// Functions
//

ISTATUS
RayTracerAllocate(
    _Out_ PRAY_TRACER *ray_tracer
    );

ISTATUS
RayTracerTraceClosestHit(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ float_t maximum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PRAY_TRACER_PROCESS_HIT_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    );

ISTATUS
RayTracerTraceClosestHitWithCoordinates(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ float_t maximum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PRAY_TRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    );

void
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAY_TRACER ray_tracer
    );

#endif // _IRIS_RAY_TRACER_