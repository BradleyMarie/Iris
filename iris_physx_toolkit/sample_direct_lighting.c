/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_direct_lighting.c

Abstract:

    Samples the brdf and light to compute an estimate the direct lighting.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/sample_direct_lighting.h"

//
// Static Functions
//

static
inline
float_t
ComputeSampleWeight(
    _In_ float_t sampled_pdf,
    _In_ float_t computed_pdf
    )
{
    assert(isfinite(sampled_pdf) && (float_t)0.0 <= sampled_pdf);
    assert(isfinite(computed_pdf) && (float_t)0.0 <= computed_pdf);

    float_t sampled_pdf_squared = sampled_pdf * sampled_pdf;
    float_t computed_pdf_squared = computed_pdf * computed_pdf;

    return sampled_pdf_squared / (sampled_pdf_squared + computed_pdf_squared);
}

static
ISTATUS
DeltaLightLighting(
    _In_ PCSPECTRUM light_spectrum,
    _In_ PCBRDF brdf,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(light_spectrum != NULL);
    assert(brdf != NULL);
    assert(PointValidate(hit_point));
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(to_light));
    assert(spectrum_compositor != NULL);
    assert(reflector_allocator != NULL);
    assert(spectrum != NULL);

    PCREFLECTOR reflector;
    ISTATUS status = BrdfComputeReflectance(brdf,
                                            to_hit_point,
                                            surface_normal,
                                            to_light,
                                            reflector_allocator,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t falloff = VectorDotProduct(shading_normal, to_light);

    status = SpectrumCompositorAttenuatedAddReflection(spectrum_compositor,
                                                       light_spectrum,
                                                       reflector,
                                                       falloff,
                                                       spectrum);

    return status;
}

static
ISTATUS
DeltaBrdfLighting(
    _In_ PCLIGHT light,
    _In_ PCREFLECTOR reflector,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing_direction,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(light != NULL);
    assert(reflector != NULL);
    assert(PointValidate(hit_point));
    assert(VectorValidate(shading_normal));
    assert(VectorValidate(outgoing_direction));
    assert(visibility_tester != NULL);
    assert(spectrum_compositor != NULL);
    assert(spectrum != NULL);

    RAY to_light = RayCreate(hit_point, outgoing_direction);

    PCSPECTRUM light_spectrum;
    ISTATUS status = LightComputeEmissive(light,
                                          to_light,
                                          visibility_tester,
                                          spectrum_compositor,
                                          &light_spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t falloff = VectorDotProduct(shading_normal, outgoing_direction);

    status = SpectrumCompositorAttenuatedAddReflection(spectrum_compositor,
                                                       light_spectrum,
                                                       reflector,
                                                       falloff,
                                                       spectrum);

    return status;
}

//
// Functions
//

ISTATUS
SampleDirectLighting(
    _In_ PCLIGHT light,
    _In_ PCBRDF brdf,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(light != NULL);
    assert(brdf != NULL);
    assert(PointValidate(hit_point));
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(shading_normal));
    assert(rng != NULL);
    assert(visibility_tester != NULL);
    assert(spectrum_compositor != NULL);
    assert(reflector_allocator != NULL);
    assert(spectrum != NULL);

    PCSPECTRUM light_sampled_spectrum;
    VECTOR3 light_sampled_direction;
    float_t light_sampled_pdf;
    ISTATUS status = LightSample(light,
                                 hit_point,
                                 visibility_tester,
                                 rng,
                                 spectrum_compositor,
                                 &light_sampled_spectrum,
                                 &light_sampled_direction,
                                 &light_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (isinf(light_sampled_pdf))
    {
        status = DeltaLightLighting(light_sampled_spectrum,
                                    brdf,
                                    hit_point,
                                    to_hit_point,
                                    surface_normal,
                                    shading_normal,
                                    light_sampled_direction,
                                    spectrum_compositor,
                                    reflector_allocator,
                                    spectrum);

        return status;
    }

    PCREFLECTOR brdf_sampled_reflector;
    VECTOR3 brdf_sampled_direction;
    float_t brdf_sampled_pdf;
    status = BrdfSample(brdf,
                        to_hit_point,
                        surface_normal,
                        rng,
                        reflector_allocator,
                        &brdf_sampled_reflector,
                        &brdf_sampled_direction,
                        &brdf_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (isinf(brdf_sampled_pdf))
    {
        status = DeltaBrdfLighting(light,
                                   brdf_sampled_reflector,
                                   hit_point,
                                   shading_normal,
                                   brdf_sampled_direction,
                                   visibility_tester,
                                   spectrum_compositor,
                                   spectrum);

        return ISTATUS_SUCCESS;
    }

    PCREFLECTOR brdf_computed_reflector;
    float_t brdf_computed_pdf;
    status = BrdfComputeReflectanceWithPdf(brdf,
                                           to_hit_point,
                                           surface_normal,
                                           light_sampled_direction,
                                           reflector_allocator,
                                           &brdf_computed_reflector,
                                           &brdf_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < brdf_computed_pdf)
    {
        float_t light_sample_falloff = 
            VectorDotProduct(shading_normal, light_sampled_direction);

        float_t light_sample_weight = ComputeSampleWeight(light_sampled_pdf,
                                                          brdf_computed_pdf);
        
        float_t light_sample_attenuation = light_sample_falloff * 
                                           light_sample_weight;

        status = 
            SpectrumCompositorAttenuatedAddReflection(spectrum_compositor,
                                                      light_sampled_spectrum,
                                                      brdf_computed_reflector,
                                                      light_sample_attenuation,
                                                      &light_sampled_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        light_sampled_spectrum = NULL;
    }

    RAY brdf_sampled_to_light = RayCreate(hit_point, brdf_sampled_direction);

    PCSPECTRUM light_computed_spectrum;
    float_t light_computed_pdf;
    status = LightComputeEmissiveWithPdf(light,
                                         brdf_sampled_to_light,
                                         visibility_tester,
                                         spectrum_compositor,
                                         &light_computed_spectrum,
                                         &light_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCSPECTRUM brdf_sampled_spectrum;
    if ((float_t)0.0 < light_computed_pdf)
    {
        float_t brdf_sample_falloff =
            VectorDotProduct(shading_normal, brdf_sampled_direction);

        float_t brdf_sample_weight = ComputeSampleWeight(brdf_sampled_pdf,
                                                         light_computed_pdf);
        
        float_t brdf_sample_attenuation = brdf_sample_falloff * 
                                          brdf_sample_weight;

        status =
            SpectrumCompositorAttenuatedAddReflection(spectrum_compositor,
                                                      light_computed_spectrum,
                                                      brdf_sampled_reflector,
                                                      brdf_sample_attenuation,
                                                      &brdf_sampled_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        brdf_sampled_spectrum = NULL;
    }

    status = SpectrumCompositorAddSpectra(spectrum_compositor,
                                          brdf_sampled_spectrum,
                                          light_sampled_spectrum,
                                          spectrum);

    return status;
}