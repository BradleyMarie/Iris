/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    sample_direct_lighting.c

Abstract:

    Samples the brdf and light to compute an estimate the direct lighting.

--*/

#include "iris_physx_toolkit/sample_direct_lighting.h"

//
// Static Functions
//

static
inline
float_t
PowerHeuristic(
    _In_ float_t sampled_pdf,
    _In_ float_t computed_pdf
    )
{
    assert(isfinite(sampled_pdf));
    assert((float_t)0.0 <= sampled_pdf);
    assert(isfinite(computed_pdf));
    assert((float_t)0.0 <= computed_pdf);

    sampled_pdf *= sampled_pdf;
    computed_pdf *= computed_pdf;

    return sampled_pdf / (sampled_pdf + computed_pdf);
}

static
ISTATUS
DeltaLightLighting(
    _In_ PCSPECTRUM light_spectrum,
    _In_ PCBRDF brdf,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(light_spectrum != NULL);
    assert(brdf != NULL);
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(to_light));
    assert(spectrum_compositor != NULL);
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    PCREFLECTOR reflector;
    ISTATUS status = BrdfComputeReflectance(brdf,
                                            to_hit_point,
                                            surface_normal,
                                            to_light,
                                            reflector_compositor,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t falloff = VectorBoundedDotProduct(shading_normal, to_light);

    status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
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

    float_t falloff = VectorBoundedDotProduct(shading_normal,
                                              outgoing_direction);

    status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                   light_spectrum,
                                                   reflector,
                                                   falloff,
                                                   spectrum);

    return status;
}

static
ISTATUS
LightLighting(
    _In_ PCSPECTRUM light_spectrum,
    _In_ float_t light_pdf,
    _In_ PCBRDF brdf,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    assert(light_spectrum != NULL);
    assert((float_t)0.0 < light_pdf);
    assert(isfinite(light_pdf));
    assert(brdf != NULL);
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(to_light));
    assert(spectrum_compositor != NULL);
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    PCREFLECTOR brdf_computed_reflector;
    float_t brdf_computed_pdf;
    ISTATUS status = BrdfComputeReflectanceWithPdf(brdf,
                                                   to_hit_point,
                                                   surface_normal,
                                                   to_light,
                                                   reflector_compositor,
                                                   &brdf_computed_reflector,
                                                   &brdf_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < brdf_computed_pdf && isfinite(brdf_computed_pdf))
    {
        float_t light_sample_falloff =
            VectorBoundedDotProduct(shading_normal, to_light);

        float_t light_sample_weight = PowerHeuristic(light_pdf, brdf_computed_pdf);

        float_t light_sample_attenuation =
            (light_sample_falloff * light_sample_weight) / light_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_spectrum,
                                                       brdf_computed_reflector,
                                                       light_sample_attenuation,
                                                       spectrum);
    }
    else
    {
        *spectrum = NULL;
    }

    return status;
}

static
ISTATUS
BrdfLighting(
    _In_ PCLIGHT light,
    _In_ PCBRDF brdf,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
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
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    PCREFLECTOR brdf_sampled_reflector;
    VECTOR3 brdf_sampled_direction;
    float_t brdf_sampled_pdf;
    ISTATUS status = BrdfSample(brdf,
                                to_hit_point,
                                surface_normal,
                                rng,
                                reflector_compositor,
                                &brdf_sampled_reflector,
                                &brdf_sampled_direction,
                                &brdf_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (brdf_sampled_pdf <= (float_t)0.0)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
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

        return status;
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

    if ((float_t)0.0 < light_computed_pdf && isfinite(light_computed_pdf))
    {
        float_t brdf_sample_falloff =
            VectorBoundedDotProduct(shading_normal, brdf_sampled_direction);
        float_t brdf_sample_weight = PowerHeuristic(brdf_sampled_pdf,
                                                    light_computed_pdf);
        float_t brdf_sample_attenuation =
            (brdf_sample_falloff * brdf_sample_weight) / brdf_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_computed_spectrum,
                                                       brdf_sampled_reflector,
                                                       brdf_sample_attenuation,
                                                       spectrum);
    }
    else
    {
        *spectrum = NULL;
    }

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
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
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
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    PCSPECTRUM light_sampled_spectrum;
    VECTOR3 light_sampled_direction;
    float_t light_sampled_pdf;
    ISTATUS status = LightSample(light,
                                 hit_point,
                                 surface_normal,
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

    if (light_sampled_pdf <= (float_t)0.0)
    {
        status = BrdfLighting(light,
                              brdf,
                              hit_point,
                              to_hit_point,
                              surface_normal,
                              shading_normal,
                              rng,
                              visibility_tester,
                              spectrum_compositor,
                              reflector_compositor,
                              spectrum);

        return status;
    }

    if (isinf(light_sampled_pdf))
    {
        status = DeltaLightLighting(light_sampled_spectrum,
                                    brdf,
                                    to_hit_point,
                                    surface_normal,
                                    shading_normal,
                                    light_sampled_direction,
                                    spectrum_compositor,
                                    reflector_compositor,
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
                        reflector_compositor,
                        &brdf_sampled_reflector,
                        &brdf_sampled_direction,
                        &brdf_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (brdf_sampled_pdf <= (float_t)0.0)
    {
        status = LightLighting(light_sampled_spectrum,
                               light_sampled_pdf,
                               brdf,
                               to_hit_point,
                               surface_normal,
                               shading_normal,
                               light_sampled_direction,
                               spectrum_compositor,
                               reflector_compositor,
                               spectrum);

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

        return status;
    }

    PCREFLECTOR brdf_computed_reflector;
    float_t brdf_computed_pdf;
    status = BrdfComputeReflectanceWithPdf(brdf,
                                           to_hit_point,
                                           surface_normal,
                                           light_sampled_direction,
                                           reflector_compositor,
                                           &brdf_computed_reflector,
                                           &brdf_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < brdf_computed_pdf && isfinite(brdf_computed_pdf))
    {
        float_t light_sample_falloff =
            VectorBoundedDotProduct(shading_normal, light_sampled_direction);

        float_t light_sample_weight = PowerHeuristic(light_sampled_pdf,
                                                     brdf_computed_pdf);

        float_t light_sample_attenuation =
            (light_sample_falloff * light_sample_weight) / light_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
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

    if ((float_t)0.0 < light_computed_pdf && isfinite(light_computed_pdf))
    {
        float_t brdf_sample_falloff =
            VectorBoundedDotProduct(shading_normal, brdf_sampled_direction);

        float_t brdf_sample_weight = PowerHeuristic(brdf_sampled_pdf,
                                                    light_computed_pdf);

        float_t brdf_sample_attenuation =
            (brdf_sample_falloff * brdf_sample_weight) / brdf_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_computed_spectrum,
                                                       brdf_sampled_reflector,
                                                       brdf_sample_attenuation,
                                                       &light_computed_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        light_computed_spectrum = NULL;
    }

    status = SpectrumCompositorAddSpectra(spectrum_compositor,
                                          light_computed_spectrum,
                                          light_sampled_spectrum,
                                          spectrum);

    return status;
}