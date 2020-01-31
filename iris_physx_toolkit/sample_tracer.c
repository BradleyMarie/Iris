/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_tracer.c

Abstract:

    Implements physx sample tracer.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/sample_tracer.h"

//
// Types
//

typedef struct _PHYSX_SAMPLE_TRACER {
    PINTEGRATOR integrator;
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
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    )
{
    PPHYSX_SAMPLE_TRACER physx_sample_tracer = (PPHYSX_SAMPLE_TRACER)context;

    ISTATUS status =
        IntegratorIntegrate(physx_sample_tracer->integrator,
                            rng,
                            *ray,
                            epsilon,
                            color);

    return status;
}

static
ISTATUS
PhysxSampleTracerDuplicate(
    _In_opt_ const void *context,
    _Out_ PSAMPLE_TRACER *duplicate
    )
{
    PCPHYSX_SAMPLE_TRACER physx_sample_tracer = (PCPHYSX_SAMPLE_TRACER)context;

    PINTEGRATOR integrator;
    ISTATUS status = IntegratorDuplicate(physx_sample_tracer->integrator,
                                         &integrator);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = PhysxSampleTracerAllocate(integrator, duplicate);

    if (status != ISTATUS_SUCCESS)
    {
        IntegratorFree(integrator);
    }

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
}

//
// Static Data
//

static const SAMPLE_TRACER_VTABLE sample_tracer_vtable = {
    PhysxSampleTracerTraceRay,
    PhysxSampleTracerDuplicate,
    PhysxSampleTracerFree
};

//
// Functions
//

ISTATUS
PhysxSampleTracerAllocate(
    _In_ PINTEGRATOR integrator,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHYSX_SAMPLE_TRACER physx_sample_tracer;
    physx_sample_tracer.integrator = integrator;

    ISTATUS status = SampleTracerAllocate(&sample_tracer_vtable,
                                          &physx_sample_tracer,
                                          sizeof(PHYSX_SAMPLE_TRACER),
                                          alignof(PHYSX_SAMPLE_TRACER),
                                          sample_tracer);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    return ISTATUS_SUCCESS;
}