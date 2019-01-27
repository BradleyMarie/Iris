/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   integrator.c

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#include <string.h>

#include "iris_physx/brdf_allocator.h"
#include "iris_physx/brdf_allocator_internal.h"
#include "iris_physx/color_integrator_internal.h"
#include "iris_physx/integrator.h"
#include "iris_physx/integrator_vtable.h"
#include "iris_physx/light_sampler_internal.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_internal.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

struct _INTEGRATOR {
    PCINTEGRATOR_VTABLE vtable;
    SHAPE_RAY_TRACER shape_ray_tracer;
    LIGHT_SAMPLER light_sampler;
    VISIBILITY_TESTER visibility_tester;
    SPECTRUM_COMPOSITOR spectrum_compositor;
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

    PINTEGRATOR result;
    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(INTEGRATOR),
                                          alignof(INTEGRATOR),
                                          (void **)&result,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->vtable = vtable;
    result->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    success = ShapeRayTracerInitialize(&result->shape_ray_tracer);
    
    if (!success)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = LightSamplerInitialize(&result->light_sampler);

    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = VisibilityTesterInitialize(&result->visibility_tester);

    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        LightSamplerDestroy(&result->light_sampler);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = SpectrumCompositorInitialize(&result->spectrum_compositor);
    
    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        LightSamplerDestroy(&result->light_sampler);
        VisibilityTesterDestroy(&result->visibility_tester);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    *integrator = result;

    return ISTATUS_SUCCESS;
}

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

    if (sample_lights_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (isinf(epsilon) || isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    ShapeRayTracerConfigure(&integrator->shape_ray_tracer,
                            trace_routine,
                            trace_context,
                            epsilon);

    LightSamplerConfigure(&integrator->light_sampler,
                          sample_lights_routine,
                          sample_lights_context);

    VisibilityTesterConfigure(&integrator->visibility_tester,
                              trace_routine,
                              trace_context,
                              epsilon);

    PREFLECTOR_COMPOSITOR reflector_compositor =
        ShapeRayTracerGetReflectorCompositor(&integrator->shape_ray_tracer);

    PCSPECTRUM spectrum;
    ISTATUS status = 
        integrator->vtable->integrate_routine(integrator->data,
                                              &ray,
                                              &integrator->shape_ray_tracer,
                                              &integrator->light_sampler,
                                              &integrator->visibility_tester,
                                              &integrator->spectrum_compositor,
                                              reflector_compositor,
                                              rng,
                                              &spectrum);

    if (status == ISTATUS_SUCCESS)
    {
        status = ColorIntegratorComputeSpectrumColor(color_integrator,
                                                     spectrum,
                                                     color);
    }

    ShapeRayTracerClear(&integrator->shape_ray_tracer);
    SpectrumCompositorClear(&integrator->spectrum_compositor);
    
    return status;
}

ISTATUS
IntegratorDuplicate(
    _In_ PCINTEGRATOR integrator,
    _Out_ PINTEGRATOR *duplicate
    )
{
    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (duplicate == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = integrator->vtable->duplicate_routine(integrator->data,
                                                           duplicate);

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
    LightSamplerDestroy(&integrator->light_sampler);
    VisibilityTesterDestroy(&integrator->visibility_tester);
    SpectrumCompositorDestroy(&integrator->spectrum_compositor);

    if (integrator->vtable->free_routine != NULL)
    {
        integrator->vtable->free_routine(integrator->data);
    }

    free(integrator);
}