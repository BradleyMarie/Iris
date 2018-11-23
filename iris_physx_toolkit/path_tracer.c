/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    path_tracer.c

Abstract:

    Implements a path_tracer.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_direct_lighting.h"

//
// Types
//

typedef struct _PATH_TRACER {
    _Field_size_(max_bounces) PCSPECTRUM *spectra;
    _Field_size_(max_bounces) PCREFLECTOR *reflectors;
    _Field_size_(max_bounces) float_t *attenuations;
    float_t min_termination_probability;
    uint8_t min_bounces;
    uint8_t max_bounces;
} PATH_TRACER, *PPATH_TRACER;

typedef const PATH_TRACER *PCPATH_TRACER;

//
// Static Functions
//

static
ISTATUS
PathTracerIntegrate(
    _In_opt_ const void *context,
    _In_ PCRAY ray,
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Inout_ PRANDOM rng,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCPATH_TRACER path_tracer = (PCPATH_TRACER)context;

    float_t path_throughput = (float_t)1.0;
    bool add_light_emissions = true;
    RAY trace_ray = *ray;
    uint8_t bounces = 0;

    for (;;)
    {
        VECTOR3 surface_normal, shading_normal;
        POINT3 hit_point;
        PCBRDF brdf;
        PCLIGHT light;
        ISTATUS status = ShapeRayTracerTrace(ray_tracer,
                                             trace_ray,
                                             &light,
                                             &brdf,
                                             &hit_point,
                                             &surface_normal,
                                             &shading_normal);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (add_light_emissions && light != NULL)
        {
            status = LightComputeEmissive(light,
                                          *ray,
                                          visibility_tester,
                                          compositor,
                                          path_tracer->spectra + bounces);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            add_light_emissions = false;
        }
        else
        {
            path_tracer->spectra[bounces] = NULL;
        }

        if (brdf == NULL)
        {
            break;
        }

        status = LightSamplerPrepareSamples(light_sampler, rng, hit_point);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        for (;;)
        {
            PCLIGHT light;
            float_t pdf;
            ISTATUS sampler_status = LightSamplerNextSample(light_sampler,
                                                            rng,
                                                            &light,
                                                            &pdf);

            if (ISTATUS_DONE < status)
            {
                return sampler_status;
            }

            PCSPECTRUM direct_lighting;
            status = SampleDirectLighting(light,
                                          brdf,
                                          hit_point,
                                          trace_ray.direction,
                                          surface_normal,
                                          shading_normal,
                                          rng,
                                          visibility_tester,
                                          compositor,
                                          allocator,
                                          &direct_lighting);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                         direct_lighting,
                                                         (float_t)1.0 / pdf,
                                                         &direct_lighting);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            status = SpectrumCompositorAddSpectra(compositor,
                                                  path_tracer->spectra[bounces],
                                                  direct_lighting,
                                                  path_tracer->spectra + bounces);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            if (sampler_status == ISTATUS_DONE)
            {
                break;
            }
        }

        if (bounces == path_tracer->max_bounces)
        {
            break;
        }

        float_t pdf;
        status = BrdfSample(brdf,
                            trace_ray.direction,
                            surface_normal,
                            rng,
                            allocator,
                            path_tracer->reflectors + bounces,
                            &trace_ray.direction,
                            &pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        float_t albedo;
        status = ReflectorGetAlbedo(path_tracer->reflectors[bounces],
                                    &albedo);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        float_t cosine_falloff = VectorDotProduct(shading_normal,
                                                  trace_ray.direction);

        path_throughput *= albedo * cosine_falloff;

        if (path_tracer->min_bounces < bounces)
        {
            float_t random_value;
            status = RandomGenerateFloat(rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &random_value);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            float_t cutoff = fmax(path_tracer->min_termination_probability,
                                  (float_t)1.0 - path_throughput);

            if (random_value < cutoff)
            {
                break;
            }

            pdf *= cutoff;
        }

        if (isinf(pdf))
        {
            path_tracer->attenuations[bounces] = (float_t)1.0;
            add_light_emissions = true;
        }
        else
        {
            path_tracer->attenuations[bounces] = (float_t)1.0 / pdf;
        }

        trace_ray.origin = hit_point;
        bounces += 1;
    }

    PCSPECTRUM result = NULL;

    for (; bounces != 0; bounces--)
    {
       ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                     result,
                                                     path_tracer->spectra[bounces],
                                                     &result);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           result,
                                                           path_tracer->reflectors[bounces - 1],
                                                           path_tracer->attenuations[bounces - 1],
                                                           &result);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                  result,
                                                  path_tracer->spectra[0],
                                                  spectrum);

    return status;
}

static
void
PathTracerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCPATH_TRACER path_tracer = (PCPATH_TRACER)context;

    free(path_tracer->spectra);
    free(path_tracer->reflectors);
    free(path_tracer->attenuations);
}

//
// Static Variables
//

static const INTEGRATOR_VTABLE path_tracer_vtable = {
    PathTracerIntegrate,
    PathTracerFree
};

//
// Functions
//

ISTATUS
PathTracerAllocate(
    _In_ uint8_t min_bounces,
    _In_ uint8_t max_bounces,
    _In_ float_t min_termination_probability,
    _Out_ PINTEGRATOR *integrator
    )
{
    if (max_bounces < min_bounces)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (!isgreaterequal(min_termination_probability, (float_t)0.0) ||
        !isless(min_termination_probability, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    PCSPECTRUM *spectra = (PCSPECTRUM*)
        calloc(max_bounces, sizeof(PCSPECTRUM));

    if (spectra == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PCREFLECTOR *reflectors = (PCREFLECTOR*)
        calloc(max_bounces, sizeof(PCREFLECTOR));

    if (reflectors == NULL)
    {
        free(spectra);
        return ISTATUS_ALLOCATION_FAILED;
    }

    float_t *attenuations = (float_t*)calloc(max_bounces, sizeof(float_t));

    if (attenuations == NULL)
    {
        free(spectra);
        free(reflectors);
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATH_TRACER path_tracer;
    path_tracer.spectra = spectra;
    path_tracer.reflectors = reflectors;
    path_tracer.attenuations = attenuations;
    path_tracer.min_termination_probability = min_termination_probability;
    path_tracer.min_bounces = min_bounces;
    path_tracer.max_bounces = max_bounces;

    ISTATUS status = IntegratorAllocate(&path_tracer_vtable,
                                        &path_tracer,
                                        sizeof(PATH_TRACER),
                                        alignof(PATH_TRACER),
                                        integrator);

    return status;
}