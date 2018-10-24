/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

   integrator.c

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#include <string.h>

#include "iris_physx/brdf_allocator.h"
#include "iris_physx/brdf_allocator_internal.h"
#include "iris_physx/integrator.h"
#include "iris_physx/integrator_vtable.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_internal.h"
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/reflector_allocator_internal.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

struct _INTEGRATOR {
    PCINTEGRATOR_VTABLE vtable;
    SHAPE_RAY_TRACER shape_ray_tracer;
    VISIBILITY_TESTER visibility_tester;
    SPECTRUM_COMPOSITOR spectrum_compositor;
    REFLECTOR_ALLOCATOR reflector_allocator;
    void *data;
};

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
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(INTEGRATOR),
                                          alignof(INTEGRATOR),
                                          (void **)integrator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*integrator)->vtable = vtable;
    (*integrator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    PINTEGRATOR result = (PINTEGRATOR)malloc(sizeof(INTEGRATOR));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = ShapeRayTracerInitialize(&result->shape_ray_tracer);
    
    if (!success)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = VisibilityTesterInitialize(&result->visibility_tester);

    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = SpectrumCompositorInitialize(&result->spectrum_compositor);
    
    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        VisibilityTesterDestroy(&result->visibility_tester);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ReflectorAllocatorInitialize(&result->reflector_allocator);

    *integrator = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
IntegratorIntegrate(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSHAPE_RAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PINTEGRATOR_TONE_MAP_ROUTINE tone_map_routine,
    _Inout_opt_ void *tone_map_context,
    _In_ RAY ray,
    _In_ PRANDOM rng,
    _In_ float_t epsilon
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

    if (tone_map_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (isinf(epsilon) || isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    ShapeRayTracerConfigure(&integrator->shape_ray_tracer,
                            trace_routine,
                            trace_context,
                            epsilon);
    
    VisibilityTesterConfigure(&integrator->visibility_tester,
                              trace_routine,
                              trace_context,
                              epsilon);
    
    PCSPECTRUM spectrum;
    ISTATUS status = 
        integrator->vtable->integrate_routine(integrator->data,
                                              &ray,
                                              &integrator->shape_ray_tracer,
                                              &integrator->visibility_tester,
                                              &integrator->spectrum_compositor,
                                              &integrator->reflector_allocator,
                                              rng,
                                              &spectrum);

    if (status == ISTATUS_SUCCESS)
    {
        status = tone_map_routine(tone_map_context, spectrum);
    }

    ShapeRayTracerClear(&integrator->shape_ray_tracer);
    SpectrumCompositorClear(&integrator->spectrum_compositor);
    ReflectorAllocatorClear(&integrator->reflector_allocator);
    
    return status;
}

void
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR integrator
    )
{
    if (integrator == NULL)
    {
        return;
    }

    ShapeRayTracerDestroy(&integrator->shape_ray_tracer);
    VisibilityTesterDestroy(&integrator->visibility_tester);
    SpectrumCompositorDestroy(&integrator->spectrum_compositor);
    ReflectorAllocatorDestroy(&integrator->reflector_allocator);

    if (integrator->vtable->free_routine != NULL)
    {
        integrator->vtable->free_routine(integrator->data);
    }

    free(integrator);
}