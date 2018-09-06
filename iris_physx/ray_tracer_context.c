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
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_context.h"
#include "iris_physx/ray_tracer_internal.h"
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/reflector_allocator_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

struct _SHAPE_RAY_TRACER_CONTEXT {
    SHAPE_RAY_TRACER shape_ray_tracer;
    PSHAPE_RAY_TRACER_TONE_MAP_ROUTINE tone_map_routine;
    void *tone_map_context;
    PRANDOM rng;
    VISIBILITY_TESTER visibility_tester;
    SPECTRUM_COMPOSITOR spectrum_compositor;
    REFLECTOR_ALLOCATOR reflector_allocator;
};

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
    _In_ float_t epsilon
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

    if (isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    SHAPE_RAY_TRACER_CONTEXT shape_ray_tracer_context;
    shape_ray_tracer_context.tone_map_routine = tone_map_routine;
    shape_ray_tracer_context.tone_map_context = tone_map_context;
    shape_ray_tracer_context.rng = rng;

    bool success = ShapeRayTracerInitialize(
        &shape_ray_tracer_context.shape_ray_tracer,
        trace_routine,
        trace_context,
        epsilon);
    
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = VisibilityTesterInitialize(
        &shape_ray_tracer_context.visibility_tester,
        trace_routine,
        trace_context,
        epsilon);

    if (!success)
    {
        ShapeRayTracerDestroy(&shape_ray_tracer_context.shape_ray_tracer);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = SpectrumCompositorInitialize(
        &shape_ray_tracer_context.spectrum_compositor);
    
    if (!success)
    {
        ShapeRayTracerDestroy(&shape_ray_tracer_context.shape_ray_tracer);
        VisibilityTesterDestroy(&shape_ray_tracer_context.visibility_tester);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ReflectorAllocatorInitialize(&shape_ray_tracer_context.reflector_allocator);

    ISTATUS status = callback(&shape_ray_tracer_context, callback_context, rng);

    ShapeRayTracerDestroy(&shape_ray_tracer_context.shape_ray_tracer);
    SpectrumCompositorDestroy(&shape_ray_tracer_context.spectrum_compositor);
    ReflectorAllocatorDestroy(&shape_ray_tracer_context.reflector_allocator);
    
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
    
    PCSPECTRUM spectrum;
    ISTATUS status = callback(callback_context,
                              &ray_tracer_context->shape_ray_tracer,
                              ray,
                              &ray_tracer_context->visibility_tester,
                              &ray_tracer_context->spectrum_compositor,
                              &ray_tracer_context->reflector_allocator,
                              ray_tracer_context->rng,
                              &spectrum);

    if (status == ISTATUS_SUCCESS)
    {
        status = ray_tracer_context->tone_map_routine(
            ray_tracer_context->tone_map_context, spectrum);
    }

    ShapeRayTracerClear(&ray_tracer_context->shape_ray_tracer);
    SpectrumCompositorClear(&ray_tracer_context->spectrum_compositor);
    ReflectorAllocatorClear(&ray_tracer_context->reflector_allocator);
    
    return status;
}