/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    path_tracer.c

Abstract:

    Implements a path_tracer.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "common/safe_math.h"
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
    _In_ PCLIGHT_SAMPLER light_sampler,
    _Inout_ PLIGHT_SAMPLE_LIST light_sample_list,
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_COMPOSITOR allocator,
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
        PCBSDF bsdf;
        PCSPECTRUM emitted_light;
        ISTATUS status = ShapeRayTracerTrace(ray_tracer,
                                             trace_ray,
                                             &emitted_light,
                                             &bsdf,
                                             &hit_point,
                                             &surface_normal,
                                             &shading_normal);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (add_light_emissions)
        {
            path_tracer->spectra[bounces] = emitted_light;
            add_light_emissions = false;
        }
        else
        {
            path_tracer->spectra[bounces] = NULL;
        }

        if (bsdf == NULL)
        {
            break;
        }

        status = LightSamplerSample(light_sampler,
                                    hit_point,
                                    rng,
                                    light_sample_list);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        size_t light_samples;
        status = LightSampleListGetSize(light_sample_list, &light_samples);

        for (size_t index = 0; index < light_samples; index++)
        {
            PCLIGHT light;
            float_t pdf;
            status = LightSampleListGetSample(light_sample_list,
                                              index,
                                              &light,
                                              &pdf);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            if (pdf <= (float_t)0.0)
            {
                continue;
            }

            PCSPECTRUM direct_lighting;
            status = SampleDirectLighting(light,
                                          bsdf,
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
        }

        if (bounces == path_tracer->max_bounces)
        {
            break;
        }

        bool transmitted;
        float_t bsdf_pdf;
        status = BsdfSample(bsdf,
                            trace_ray.direction,
                            surface_normal,
                            rng,
                            allocator,
                            path_tracer->reflectors + bounces,
                            &transmitted,
                            &trace_ray.direction,
                            &bsdf_pdf);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (bsdf_pdf <= (float_t)0.0)
        {
            break;
        }

        float_t albedo;
        status = ReflectorGetAlbedo(path_tracer->reflectors[bounces],
                                    &albedo);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        float_t attenuation = VectorPositiveDotProduct(shading_normal,
                                                       trace_ray.direction,
                                                       transmitted);
        path_throughput *= albedo * attenuation;

        if (isfinite(bsdf_pdf))
        {
            path_throughput /= bsdf_pdf;
            attenuation /= bsdf_pdf;
        }

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

            float_t roulette_pdf = (float_t)1.0 - cutoff;
            attenuation /= roulette_pdf;
            path_throughput /= roulette_pdf;
        }

        path_tracer->attenuations[bounces] = attenuation;

        if (isinf(bsdf_pdf))
        {
            add_light_emissions = true;
        }

        trace_ray.origin = hit_point;
        bounces += 1;
    }

    for (; bounces != 0; bounces--)
    {
        ISTATUS status = SpectrumCompositorAttenuateReflection(
            compositor, 
            path_tracer->spectra[bounces],
            path_tracer->reflectors[bounces - 1],
            path_tracer->attenuations[bounces - 1],
            path_tracer->spectra + bounces);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAddSpectra(
            compositor,
            path_tracer->spectra[bounces],
            path_tracer->spectra[bounces - 1],
            path_tracer->spectra + bounces - 1);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    *spectrum = path_tracer->spectra[0];

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PathTracerDuplicate(
    _In_opt_ const void *context,
    _Out_ PINTEGRATOR *duplicate
    )
{
    PCPATH_TRACER path_tracer = (PCPATH_TRACER)context;

    ISTATUS status =
        PathTracerAllocate(path_tracer->min_bounces,
                           path_tracer->max_bounces,
                           path_tracer->min_termination_probability,
                           duplicate);

    return status;
}

static
void
PathTracerFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPATH_TRACER path_tracer = (PPATH_TRACER)context;

    free(path_tracer->spectra);
    free(path_tracer->reflectors);
    free(path_tracer->attenuations);
}

//
// Static Variables
//

static const INTEGRATOR_VTABLE path_tracer_vtable = {
    PathTracerIntegrate,
    PathTracerDuplicate,
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

    if (!isfinite(min_termination_probability) ||
        min_termination_probability < (float_t)0.0 ||
        (float_t)1.0 < min_termination_probability)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    size_t num_spectra;
    bool success = CheckedAddSizeT(max_bounces, 1, &num_spectra);
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PCSPECTRUM *spectra = (PCSPECTRUM*)
        calloc(num_spectra, sizeof(PCSPECTRUM));

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