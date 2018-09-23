/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator.h

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_
#define _IRIS_PHYSX_INTEGRATOR_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/visibility_tester.h"

//
// Types
//

typedef
ISTATUS 
(*PSHAPE_RAY_TRACER_TRACE_ROUTINE)(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    );

typedef struct _INTEGRATOR INTEGRATOR, *PINTEGRATOR;
typedef const INTEGRATOR *PCINTEGRATOR;

typedef
ISTATUS
(*PINTEGRATOR_INTEGRATE_ROUTINE)(
    _In_opt_ const void *context,
    _In_ PCRAY ray,
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Inout_ PRANDOM rng,
    _Out_ PCSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PINTEGRATOR_TONE_MAP_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCSPECTRUM spectrum
    );

//
// Functions
//

ISTATUS
IntegratorAllocate(
    _Out_ PINTEGRATOR *integrator
    );

ISTATUS
IntegratorIntegrate(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PINTEGRATOR_INTEGRATE_ROUTINE integrate_routine,
    _In_opt_ const void *integrate_context,
    _In_ PINTEGRATOR_TONE_MAP_ROUTINE tone_map_routine,
    _Inout_opt_ void *tone_map_context,
    _In_ RAY ray,
    _In_ PRANDOM rng,
    _In_ float_t epsilon
    );

void
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR integrator
    );
    
#endif // _IRIS_PHYSX_INTEGRATOR_