/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    physx_sample_tracer.c

Abstract:

    Implements physx sample tracer.

--*/

#include <stdalign.h>

#include "iris_physx_camera_toolkit/physx_sample_tracer.h"

//
// Types
//

typedef struct _PHYSX_SAMPLE_TRACER {
    PINTEGRATOR integrator;
    PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine;
    PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine;
    void* light_sampler_context;
    PTONE_MAPPER tone_mapper;
} PHYSX_SAMPLE_TRACER, *PPHYSX_SAMPLE_TRACER;

typedef const PHYSX_SAMPLE_TRACER *PCPHYSX_SAMPLE_TRACER;

//
// Static Functions
//

static
ISTATUS
PhysxSampleTracerTraceRay(
    _In_opt_ void *context,
    _In_ PCRAY ray,
    _In_ PRANDOM rng,
    _In_ float_t epsilon
    )
{
    PPHYSX_SAMPLE_TRACER physx_sample_tracer = (PPHYSX_SAMPLE_TRACER)context;

    ISTATUS status = 
        IntegratorIntegrate(physx_sample_tracer->integrator,
                            physx_sample_tracer->trace_routine,
                            physx_sample_tracer->trace_context,
                            physx_sample_tracer->prepare_samples_routine,
                            physx_sample_tracer->next_sample_routine,
                            physx_sample_tracer->light_sampler_context,
                            *ray,
                            rng,
                            epsilon,
                            physx_sample_tracer->tone_mapper);

    return status;
}

static
ISTATUS
PhysxSampleTracerToneMap(
    _In_opt_ void *context,
    _Out_ PCOLOR3 color
    )
{
    PPHYSX_SAMPLE_TRACER physx_sample_tracer = (PPHYSX_SAMPLE_TRACER)context;

    ISTATUS status = ToneMapperComputeTone(physx_sample_tracer->tone_mapper,
                                           color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ToneMapperClear(physx_sample_tracer->tone_mapper);

    return status;
}

static
void
PhysxSampleTracerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPHYSX_SAMPLE_TRACER physx_sample_tracer = (PPHYSX_SAMPLE_TRACER)context;

    IntegratorFree(physx_sample_tracer->integrator);
    ToneMapperFree(physx_sample_tracer->tone_mapper);
}

//
// Static Data
//

static const SAMPLE_TRACER_VTABLE sample_tracer_vtable = {
    PhysxSampleTracerTraceRay,
    PhysxSampleTracerToneMap,
    PhysxSampleTracerFree
};

//
// Functions
//

ISTATUS
PhysxSampleTracerAllocate(
    _In_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_opt_ PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine,
    _In_ PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine,
    _Inout_opt_ void* light_sampler_context,
    _In_ PTONE_MAPPER tone_mapper,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (trace_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (next_sample_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (tone_mapper == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    PHYSX_SAMPLE_TRACER physx_sample_tracer;
    physx_sample_tracer.integrator = integrator;
    physx_sample_tracer.trace_routine = trace_routine;
    physx_sample_tracer.trace_context = trace_context;
    physx_sample_tracer.prepare_samples_routine = prepare_samples_routine;
    physx_sample_tracer.next_sample_routine = next_sample_routine;
    physx_sample_tracer.light_sampler_context = light_sampler_context;
    physx_sample_tracer.tone_mapper = tone_mapper;

    ISTATUS status = SampleTracerAllocate(&sample_tracer_vtable,
                                          &physx_sample_tracer,
                                          sizeof(PHYSX_SAMPLE_TRACER),
                                          alignof(PHYSX_SAMPLE_TRACER),
                                          sample_tracer);

    return status;
}