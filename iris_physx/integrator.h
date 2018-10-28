/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator.h

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_
#define _IRIS_PHYSX_INTEGRATOR_

#include "iris_physx/integrator_vtable.h"

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

typedef
ISTATUS
(*PINTEGRATOR_TONE_MAP_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCSPECTRUM spectrum
    );

typedef struct _INTEGRATOR INTEGRATOR, *PINTEGRATOR;
typedef const INTEGRATOR *PCINTEGRATOR;

//
// Functions
//

ISTATUS
IntegratorAllocate(
    _In_ PCINTEGRATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PINTEGRATOR *integrator
    );

ISTATUS
IntegratorIntegrate(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine,
    _In_ PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine,
    _Inout_opt_ void* light_sampler_context,
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