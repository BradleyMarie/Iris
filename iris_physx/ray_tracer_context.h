/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray_tracer_context.h

Abstract:

    Sets up and manages the lifetimes of all of the state needed to trace a 
    a scene built from the Iris Physx primitives.

--*/

#ifndef _IRIS_PHYSX_RAY_TRACER_CONTEXT_
#define _IRIS_PHYSX_RAY_TRACER_CONTEXT_

#include "iris_advanced/iris_advanced.h"
#include "iris_spectrum/iris_spectrum.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/ray_tracer.h"

//
// Types
//

typedef struct _SHAPE_RAY_TRACER_CONTEXT SHAPE_RAY_TRACER_CONTEXT;
typedef SHAPE_RAY_TRACER_CONTEXT *PSHAPE_RAY_TRACER_CONTEXT;
typedef const SHAPE_RAY_TRACER_CONTEXT *PCSHAPE_RAY_TRACER_CONTEXT;

typedef
ISTATUS
(*PSHAPE_RAY_TRACER_CONTEXT_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PSHAPE_RAY_TRACER_CONTEXT ray_tracer_context,
    _In_ PRANDOM rng
    );

typedef
ISTATUS
(*PSHAPE_RAY_TRACER_LIFETIME_ROUTINE)(
    _In_opt_ const void *context,
    _In_ PSHAPE_RAY_TRACER ray_tracer,
    _In_ RAY ray,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Inout_ PRANDOM rng,
    _Out_ PCSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PSHAPE_RAY_TRACER_TONE_MAP_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCSPECTRUM spectrum
    );

typedef
ISTATUS 
(*PSHAPE_RAY_TRACER_TRACE_ROUTINE)(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    );

//
// Functions
//

ISTATUS
ShapeRayTracerContextCreate(
    _In_ PSHAPE_RAY_TRACER_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PSHAPE_RAY_TRACER_TONE_MAP_ROUTINE tone_map_routine,
    _Inout_opt_ void *tone_map_context,
    _In_ PRANDOM rng,
    _In_ float_t minimum_distance
    );

ISTATUS
ShapeRayTracerCreate(
    _Inout_ PSHAPE_RAY_TRACER_CONTEXT ray_tracer_context,
    _In_ PSHAPE_RAY_TRACER_LIFETIME_ROUTINE callback,
    _In_opt_ const void *callback_context,
    _In_ RAY ray
    );
    
#endif // _IRIS_PHYSX_RAY_TRACER_CONTEXT_