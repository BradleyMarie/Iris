/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

   ray_tracer_context.c

Abstract:

    A BRDF allocator context mananges the lifetime of a BRDF allocator in order
    to allow memory allocated by the allocator to be reused across each lifetime
    of the allocator.

--*/

#include "iris_physx/brdf_allocator.h"
#include "iris_physx/brdf_allocator_internal.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_context.h"
#include "iris_physx/ray_tracer_internal.h"

//
// Types
//

struct _SHAPE_RAY_TRACER_CONTEXT {
    PSHAPE_RAY_TRACER_CONTEXT_LIFETIME_ROUTINE callback;
    void *callback_context;
    PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    PSHAPE_RAY_TRACER_TONE_MAP_ROUTINE tone_map_routine;
    void *tone_map_context;
    PRANDOM rng;
    float_t minimum_distance;
    PSPECTRUM_COMPOSITOR_CONTEXT spectrum_compositor_context;
    PREFLECTOR_ALLOCATOR_CONTEXT reflector_allocator_context;
    PRAY_TRACER ray_tracer;
    BRDF_ALLOCATOR brdf_allocator;
    PSHAPE_RAY_TRACER_LIFETIME_ROUTINE ray_tracer_callback;
    const void *ray_tracer_callback_context;
    RAY ray;
    PSPECTRUM_COMPOSITOR spectrum_compositor;
};

//
// Static Functions
//

static
ISTATUS
ShapeRayTracerContextReflectorAllocatorContextLifetime(
    _Inout_ void *context,
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT allocator_context
    )
{
    PSHAPE_RAY_TRACER_CONTEXT data = (PSHAPE_RAY_TRACER_CONTEXT)context;

    data->reflector_allocator_context = allocator_context;

    ISTATUS status = RayTracerAllocate(&data->ray_tracer);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    BrdfAllocatorInitialize(&data->brdf_allocator);

    status = data->callback(data->callback_context, data, data->rng);

    RayTracerFree(data->ray_tracer);
    BrdfAllocatorDestroy(&data->brdf_allocator);

    return status;
}

static
ISTATUS
ShapeRayTracerContextSpectrumCompositorContextLifetime(
    _Inout_ void *context,
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context
    )
{
    PSHAPE_RAY_TRACER_CONTEXT data = (PSHAPE_RAY_TRACER_CONTEXT)context;

    data->spectrum_compositor_context = compositor_context;

    ISTATUS status = ReflectorAllocatorContextCreate(
        ShapeRayTracerContextReflectorAllocatorContextLifetime, context);

    return status;
}

static
ISTATUS
ShapeRayTracerContextReflectorAllocatorLifetime(
    _Inout_ void *context,
    _In_ PREFLECTOR_ALLOCATOR allocator
    )
{
    PSHAPE_RAY_TRACER_CONTEXT data = (PSHAPE_RAY_TRACER_CONTEXT)context;

    SHAPE_RAY_TRACER ray_tracer;
    ray_tracer.ray_tracer = data->ray_tracer;
    ray_tracer.minimum_distance = data->minimum_distance;
    ray_tracer.trace_routine = data->trace_routine;
    ray_tracer.trace_context = data->trace_context;
    ray_tracer.brdf_allocator = &data->brdf_allocator;

    PCSPECTRUM spectrum;
    ISTATUS status = 
        data->ray_tracer_callback(data->ray_tracer_callback_context,
                                  &ray_tracer,
                                  data->ray,
                                  data->spectrum_compositor,
                                  allocator,
                                  data->rng,
                                  &spectrum);
    
    if (status == ISTATUS_SUCCESS)
    {
        status = data->tone_map_routine(data->tone_map_context, spectrum);
    }

    BrdfAllocatorClear(&data->brdf_allocator);

    return status;
}

static
ISTATUS
ShapeRayTracerContextSpectrumCompositorLifetime(
    _Inout_ void *context,
    _In_ PSPECTRUM_COMPOSITOR compositor
    )
{
    PSHAPE_RAY_TRACER_CONTEXT data = (PSHAPE_RAY_TRACER_CONTEXT)context;

    data->spectrum_compositor = compositor;

    ISTATUS status = ReflectorAllocatorCreate(
        data->reflector_allocator_context, 
        ShapeRayTracerContextReflectorAllocatorLifetime,
        context);

    return status;
}

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
    )
{
    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (trace_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (tone_map_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (!isfinite(minimum_distance) || minimum_distance < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    SHAPE_RAY_TRACER_CONTEXT shape_ray_tracer_context;
    shape_ray_tracer_context.callback = callback;
    shape_ray_tracer_context.callback_context = callback_context;
    shape_ray_tracer_context.trace_routine = trace_routine;
    shape_ray_tracer_context.trace_context = trace_context;
    shape_ray_tracer_context.tone_map_routine = tone_map_routine;
    shape_ray_tracer_context.tone_map_context = tone_map_context;
    shape_ray_tracer_context.rng = rng;
    shape_ray_tracer_context.minimum_distance = minimum_distance;
    shape_ray_tracer_context.spectrum_compositor_context = NULL;
    shape_ray_tracer_context.reflector_allocator_context = NULL;
    shape_ray_tracer_context.ray_tracer = NULL;
    shape_ray_tracer_context.ray_tracer_callback = NULL;
    shape_ray_tracer_context.ray_tracer_callback_context = NULL;
    shape_ray_tracer_context.spectrum_compositor = NULL;

    ISTATUS status = SpectrumCompositorContextCreate(
        ShapeRayTracerContextSpectrumCompositorContextLifetime, 
        &shape_ray_tracer_context);
    
    return status;
}

ISTATUS
ShapeRayTracerCreate(
    _Inout_ PSHAPE_RAY_TRACER_CONTEXT ray_tracer_context,
    _In_ PSHAPE_RAY_TRACER_LIFETIME_ROUTINE callback,
    _In_opt_ const void *callback_context,
    _In_ RAY ray
    )
{
    if (ray_tracer_context == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ray_tracer_context->ray_tracer_callback = callback;
    ray_tracer_context->ray_tracer_callback_context = callback_context;
    ray_tracer_context->ray = ray;

    ISTATUS status = SpectrumCompositorCreate(
        ray_tracer_context->spectrum_compositor_context,
        ShapeRayTracerContextSpectrumCompositorLifetime,
        ray_tracer_context);
    
    return status;
}