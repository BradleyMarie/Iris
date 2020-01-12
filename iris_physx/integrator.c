/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   integrator.c

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#include <string.h>

#include "iris_physx/bsdf_allocator.h"
#include "iris_physx/bsdf_allocator_internal.h"
#include "iris_physx/color_cache.h"
#include "iris_physx/color_cache_internal.h"
#include "iris_physx/color_integrator_internal.h"
#include "iris_physx/integrator.h"
#include "iris_physx/integrator_vtable.h"
#include "iris_physx/light_sample_list_internal.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_internal.h"
#include "iris_physx/scene_internal.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

struct _INTEGRATOR {
    PCINTEGRATOR_VTABLE vtable;
    SHAPE_RAY_TRACER shape_ray_tracer;
    LIGHT_SAMPLE_LIST light_sample_list;
    VISIBILITY_TESTER visibility_tester;
    SPECTRUM_COMPOSITOR spectrum_compositor;
    COLOR_CACHE color_cache;
    PSCENE scene;
    PLIGHT_SAMPLER light_sampler;
    PCOLOR_INTEGRATOR color_integrator;
    void *data;
};

//
// Static Functions
//

static
ISTATUS
IntegratorIntegrateInternal(
    _Inout_ PINTEGRATOR integrator,
    _In_ PCSCENE scene,
    _In_ PCLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(spectrum != NULL);

    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (isinf(epsilon) || isless(epsilon, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    ShapeRayTracerConfigure(&integrator->shape_ray_tracer,
                            scene->vtable->trace_routine,
                            scene->data,
                            epsilon);

    VisibilityTesterConfigure(&integrator->visibility_tester,
                              scene->vtable->trace_routine,
                              scene->data,
                              epsilon);

    LightSampleListClear(&integrator->light_sample_list);
    SpectrumCompositorClear(&integrator->spectrum_compositor);

    PREFLECTOR_COMPOSITOR reflector_compositor =
        ShapeRayTracerGetReflectorCompositor(&integrator->shape_ray_tracer);

    ISTATUS status =
        integrator->vtable->integrate_routine(integrator->data,
                                              &ray,
                                              light_sampler,
                                              &integrator->light_sample_list,
                                              &integrator->shape_ray_tracer,
                                              &integrator->visibility_tester,
                                              &integrator->spectrum_compositor,
                                              reflector_compositor,
                                              rng,
                                              spectrum);

    return status;
}

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

    success = LightSampleListInitialize(&result->light_sample_list);

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
        LightSampleListDestroy(&result->light_sample_list);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = SpectrumCompositorInitialize(&result->spectrum_compositor);
    
    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        LightSampleListDestroy(&result->light_sample_list);
        VisibilityTesterDestroy(&result->visibility_tester);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = ColorCacheInitialize(&result->color_cache);

    if (!success)
    {
        ShapeRayTracerDestroy(&result->shape_ray_tracer);
        LightSampleListDestroy(&result->light_sample_list);
        VisibilityTesterDestroy(&result->visibility_tester);
        SpectrumCompositorDestroy(&result->spectrum_compositor);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->scene = NULL;
    result->light_sampler = NULL;
    result->color_integrator = NULL;

    *integrator = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
IntegratorPrepare(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSCENE scene,
    _In_ PLIGHT_SAMPLER light_sampler,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_ bool precompute_colors
    )
{
    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light_sampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = ColorCacheReset(&integrator->color_cache,
                                     color_integrator);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SceneRelease(integrator->scene);
    LightSamplerRelease(integrator->light_sampler);
    ColorIntegratorRelease(integrator->color_integrator);

    integrator->scene = scene;
    integrator->light_sampler = light_sampler;
    integrator->color_integrator = color_integrator;

    SceneRetain(scene);
    LightSamplerRetain(light_sampler);
    ColorIntegratorRetain(color_integrator);

    status = SceneCacheColors(scene, &integrator->color_cache);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LightSamplerCacheColors(light_sampler, &integrator->color_cache);

    return status;
}

ISTATUS
IntegratorIntegrate(
    _Inout_ PINTEGRATOR integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    )
{
    PCSPECTRUM spectrum;
    ISTATUS status = IntegratorIntegrateInternal(integrator,
                                                 integrator->scene,
                                                 integrator->light_sampler,
                                                 rng,
                                                 ray,
                                                 epsilon,
                                                 &spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ColorIntegratorComputeSpectrumColorFast(integrator->color_integrator,
                                                     spectrum,
                                                     color);

    return status;
}

ISTATUS
IntegratorIntegrateSpectral(
    _Inout_ PINTEGRATOR integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    )
{
    PCSPECTRUM spectrum;
    ISTATUS status = IntegratorIntegrateInternal(integrator,
                                                 integrator->scene,
                                                 integrator->light_sampler,
                                                 rng,
                                                 ray,
                                                 epsilon,
                                                 &spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (spectrum == NULL)
    {
        *color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        return ISTATUS_SUCCESS;
    }

    status = ColorIntegratorComputeSpectrumColor(integrator->color_integrator,
                                                 spectrum,
                                                 color);
    
    return status;
}

ISTATUS
IntegratorDuplicate(
    _In_ PINTEGRATOR integrator,
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

    PINTEGRATOR result;
    ISTATUS status = integrator->vtable->duplicate_routine(integrator->data,
                                                           &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    result->scene = integrator->scene;
    result->light_sampler = integrator->light_sampler;
    result->color_integrator = integrator->color_integrator;

    ColorCacheShareWith(&integrator->color_cache, &result->color_cache);
    SceneRetain(integrator->scene);
    LightSamplerRetain(integrator->light_sampler);
    ColorIntegratorRetain(integrator->color_integrator);

    *duplicate = result;

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
    LightSampleListDestroy(&integrator->light_sample_list);
    VisibilityTesterDestroy(&integrator->visibility_tester);
    SpectrumCompositorDestroy(&integrator->spectrum_compositor);
    ColorCacheDestroy(&integrator->color_cache);

    if (integrator->vtable->free_routine != NULL)
    {
        integrator->vtable->free_routine(integrator->data);
    }

    free(integrator);
}