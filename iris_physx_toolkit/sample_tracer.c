/*++

Copyright (c) 2019 Brad Weinberger

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
    PCSCENE scene;
    PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine;
    const void* sample_lights_context;
    PCOLOR_INTEGRATOR color_integrator;
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
                            physx_sample_tracer->scene,
                            physx_sample_tracer->sample_lights_routine,
                            physx_sample_tracer->sample_lights_context,
                            physx_sample_tracer->color_integrator,
                            rng,
                            *ray,
                            epsilon,
                            color);

    return status;
}

static
ISTATUS
PhysxSpectralSampleTracerTraceRay(
    _In_opt_ void *context,
    _In_ PCRAY ray,
    _In_ PRANDOM rng,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    )
{
    PPHYSX_SAMPLE_TRACER physx_sample_tracer = (PPHYSX_SAMPLE_TRACER)context;

    ISTATUS status =
        IntegratorIntegrateSpectral(physx_sample_tracer->integrator,
                                    physx_sample_tracer->scene,
                                    physx_sample_tracer->sample_lights_routine,
                                    physx_sample_tracer->sample_lights_context,
                                    physx_sample_tracer->color_integrator,
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

    status =
        PhysxSampleTracerAllocate(integrator,
                                  physx_sample_tracer->scene,
                                  physx_sample_tracer->sample_lights_routine,
                                  physx_sample_tracer->sample_lights_context,
                                  physx_sample_tracer->color_integrator,
                                  duplicate);

    if (status != ISTATUS_SUCCESS)
    {
        IntegratorFree(integrator);
    }

    return status;
}

static
ISTATUS
PhysxSpectralSampleTracerDuplicate(
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

    status =
        PhysxSpectralSampleTracerAllocate(integrator,
                                          physx_sample_tracer->scene,
                                          physx_sample_tracer->sample_lights_routine,
                                          physx_sample_tracer->sample_lights_context,
                                          physx_sample_tracer->color_integrator,
                                          duplicate);

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

static
ISTATUS
PhysxSampleTracerAllocateInternal(
    _In_ PCSAMPLE_TRACER_VTABLE vtable,
    _In_ PINTEGRATOR integrator,
    _In_ PCSCENE scene,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (sample_lights_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (sample_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PHYSX_SAMPLE_TRACER physx_sample_tracer;
    physx_sample_tracer.integrator = integrator;
    physx_sample_tracer.scene = scene;
    physx_sample_tracer.sample_lights_routine = sample_lights_routine;
    physx_sample_tracer.sample_lights_context = sample_lights_context;
    physx_sample_tracer.color_integrator = color_integrator;

    ISTATUS status = SampleTracerAllocate(vtable,
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

//
// Static Data
//

static const SAMPLE_TRACER_VTABLE sample_tracer_vtable = {
    PhysxSampleTracerTraceRay,
    PhysxSampleTracerDuplicate,
    PhysxSampleTracerFree
};

static const SAMPLE_TRACER_VTABLE spectal_sample_tracer_vtable = {
    PhysxSpectralSampleTracerTraceRay,
    PhysxSpectralSampleTracerDuplicate,
    PhysxSampleTracerFree
};

//
// Functions
//

ISTATUS
PhysxSampleTracerAllocate(
    _In_ PINTEGRATOR integrator,
    _In_ PCSCENE scene,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    ISTATUS status =
        PhysxSampleTracerAllocateInternal(&sample_tracer_vtable,
                                          integrator,
                                          scene,
                                          sample_lights_routine,
                                          sample_lights_context,
                                          color_integrator,
                                          sample_tracer);

    return status;
}

ISTATUS
PhysxSpectralSampleTracerAllocate(
    _In_ PINTEGRATOR integrator,
    _In_ PCSCENE scene,
    _In_ PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE sample_lights_routine,
    _In_opt_ const void* sample_lights_context,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    ISTATUS status =
        PhysxSampleTracerAllocateInternal(&spectal_sample_tracer_vtable,
                                          integrator,
                                          scene,
                                          sample_lights_routine,
                                          sample_lights_context,
                                          color_integrator,
                                          sample_tracer);

    return status;
}