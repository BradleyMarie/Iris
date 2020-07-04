/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_direct_lighting.c

Abstract:

    Samples the bsdf and light to compute an estimate the direct lighting.

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
    _In_ PCBSDF bsdf,
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
    assert(bsdf != NULL);
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(to_light));
    assert(spectrum_compositor != NULL);
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    float_t falloff = VectorDotProduct(shading_normal, to_light);

    bool transmitted;
    if (falloff < (float_t)0.0)
    {
        falloff = -falloff;
        transmitted = true;
    }
    else
    {
        transmitted = false;
    }

    PCREFLECTOR reflector;
    ISTATUS status = BsdfComputeReflectance(bsdf,
                                            to_hit_point,
                                            surface_normal,
                                            to_light,
                                            transmitted,
                                            reflector_compositor,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                   light_spectrum,
                                                   reflector,
                                                   falloff,
                                                   spectrum);

    return status;
}

static
ISTATUS
DeltaBsdfLighting(
    _In_ PCLIGHT light,
    _In_ PCREFLECTOR reflector,
    _In_ bool transmitted,
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

    status = SpectrumCompositorReflect(spectrum_compositor,
                                       light_spectrum,
                                       reflector,
                                       spectrum);

    return status;
}

static
ISTATUS
BsdfLighting(
    _In_ PCLIGHT light,
    _In_ PCBSDF bsdf,
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
    assert(bsdf != NULL);
    assert(PointValidate(hit_point));
    assert(VectorValidate(to_hit_point));
    assert(VectorValidate(surface_normal));
    assert(VectorValidate(shading_normal));
    assert(rng != NULL);
    assert(visibility_tester != NULL);
    assert(spectrum_compositor != NULL);
    assert(reflector_compositor != NULL);
    assert(spectrum != NULL);

    PCREFLECTOR bsdf_sampled_reflector;
    bool bsdf_sampled_transmitted;
    VECTOR3 bsdf_sampled_direction;
    float_t bsdf_sampled_pdf;
    ISTATUS status = BsdfSample(bsdf,
                                to_hit_point,
                                surface_normal,
                                rng,
                                reflector_compositor,
                                &bsdf_sampled_reflector,
                                &bsdf_sampled_transmitted,
                                &bsdf_sampled_direction,
                                &bsdf_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (bsdf_sampled_pdf <= (float_t)0.0)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (isinf(bsdf_sampled_pdf))
    {
        status = DeltaBsdfLighting(light,
                                   bsdf_sampled_reflector,
                                   bsdf_sampled_transmitted,
                                   hit_point,
                                   shading_normal,
                                   bsdf_sampled_direction,
                                   visibility_tester,
                                   spectrum_compositor,
                                   spectrum);

        return status;
    }

    RAY bsdf_sampled_to_light = RayCreate(hit_point, bsdf_sampled_direction);

    PCSPECTRUM light_computed_spectrum;
    float_t light_computed_pdf;
    status = LightComputeEmissiveWithPdf(light,
                                         bsdf_sampled_to_light,
                                         visibility_tester,
                                         spectrum_compositor,
                                         &light_computed_spectrum,
                                         &light_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < light_computed_pdf)
    {
        float_t bsdf_sample_falloff =
            VectorPositiveDotProduct(shading_normal,
                                     bsdf_sampled_direction,
                                     bsdf_sampled_transmitted);

        float_t bsdf_sample_weight = PowerHeuristic(bsdf_sampled_pdf,
                                                    light_computed_pdf);
        float_t bsdf_sample_attenuation =
            (bsdf_sample_falloff * bsdf_sample_weight) / bsdf_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_computed_spectrum,
                                                       bsdf_sampled_reflector,
                                                       bsdf_sample_attenuation,
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
    _In_ PCBSDF bsdf,
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
    assert(bsdf != NULL);
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
        status = BsdfLighting(light,
                              bsdf,
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
                                    bsdf,
                                    to_hit_point,
                                    surface_normal,
                                    shading_normal,
                                    light_sampled_direction,
                                    spectrum_compositor,
                                    reflector_compositor,
                                    spectrum);

        return status;
    }

    PCREFLECTOR bsdf_sampled_reflector;
    bool bsdf_sampled_transmitted;
    VECTOR3 bsdf_sampled_direction;
    float_t bsdf_sampled_pdf;
    status = BsdfSample(bsdf,
                        to_hit_point,
                        surface_normal,
                        rng,
                        reflector_compositor,
                        &bsdf_sampled_reflector,
                        &bsdf_sampled_transmitted,
                        &bsdf_sampled_direction,
                        &bsdf_sampled_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (isinf(bsdf_sampled_pdf))
    {
        status = DeltaBsdfLighting(light,
                                   bsdf_sampled_reflector,
                                   bsdf_sampled_transmitted,
                                   hit_point,
                                   shading_normal,
                                   bsdf_sampled_direction,
                                   visibility_tester,
                                   spectrum_compositor,
                                   spectrum);

        return status;
    }

    float_t light_sample_falloff = VectorDotProduct(shading_normal,
                                                    light_sampled_direction);

    bool bsdf_computed_transmitted;
    if (light_sample_falloff < (float_t)0.0)
    {
        light_sample_falloff = -light_sample_falloff;
        bsdf_computed_transmitted = true;
    }
    else
    {
        bsdf_computed_transmitted = false;
    }

    PCREFLECTOR bsdf_computed_reflector;
    float_t bsdf_computed_pdf;
    status = BsdfComputeReflectanceWithPdf(bsdf,
                                           to_hit_point,
                                           surface_normal,
                                           light_sampled_direction,
                                           bsdf_computed_transmitted,
                                           reflector_compositor,
                                           &bsdf_computed_reflector,
                                           &bsdf_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < bsdf_computed_pdf)
    {
        float_t light_sample_falloff =
            VectorPositiveDotProduct(shading_normal,
                                     light_sampled_direction,
                                     bsdf_computed_transmitted);

        float_t light_sample_weight = PowerHeuristic(light_sampled_pdf,
                                                     bsdf_computed_pdf);

        float_t light_sample_attenuation =
            (light_sample_falloff * light_sample_weight) / light_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_sampled_spectrum,
                                                       bsdf_computed_reflector,
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

    RAY bsdf_sampled_to_light = RayCreate(hit_point, bsdf_sampled_direction);

    PCSPECTRUM light_computed_spectrum;
    float_t light_computed_pdf;
    status = LightComputeEmissiveWithPdf(light,
                                         bsdf_sampled_to_light,
                                         visibility_tester,
                                         spectrum_compositor,
                                         &light_computed_spectrum,
                                         &light_computed_pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)0.0 < light_computed_pdf)
    {
        float_t bsdf_sample_falloff =
            VectorPositiveDotProduct(shading_normal,
                                     bsdf_sampled_direction,
                                     bsdf_sampled_transmitted);

        float_t bsdf_sample_weight = PowerHeuristic(bsdf_sampled_pdf,
                                                    light_computed_pdf);

        float_t bsdf_sample_attenuation =
            (bsdf_sample_falloff * bsdf_sample_weight) / bsdf_sampled_pdf;

        status = SpectrumCompositorAttenuateReflection(spectrum_compositor,
                                                       light_computed_spectrum,
                                                       bsdf_sampled_reflector,
                                                       bsdf_sample_attenuation,
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