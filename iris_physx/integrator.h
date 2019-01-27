/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    integrator.h

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_
#define _IRIS_PHYSX_INTEGRATOR_

#include "iris_physx/color_integrator.h"
#include "iris_physx/light_sampler_vtable.h"
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
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCCOLOR_INTEGRATOR color_integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    );

ISTATUS
IntegratorIntegrateSpectral(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCCOLOR_INTEGRATOR color_integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    );

ISTATUS
IntegratorDuplicate(
    _In_ PCINTEGRATOR integrator,
    _Out_ PINTEGRATOR *duplicate
    );

void
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR integrator
    );
    
#endif // _IRIS_PHYSX_INTEGRATOR_