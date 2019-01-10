/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor.c

Abstract:

    Enables the composition of spectra and reflectors into new spectra.

--*/

#include "iris_physx/spectrum_compositor.h"

#include <assert.h>

#include "iris_physx/reflector_internal.h"
#include "iris_physx/spectrum_compositor_internal.h"

//
// Static Functions
//

static
ISTATUS
AttenuatedSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(intensity != NULL);

    PCATTENUATED_SPECTRUM attenuated_spectrum = (PCATTENUATED_SPECTRUM) context;

    float_t output_intensity;
    ISTATUS Status = SpectrumSampleInline(attenuated_spectrum->spectrum,
                                          wavelength,
                                          &output_intensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *intensity = output_intensity * attenuated_spectrum->attenuation;
    return ISTATUS_SUCCESS; 
}

static
ISTATUS
SumSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(intensity != NULL);

    PCSUM_SPECTRUM sum_spectrum = (PCSUM_SPECTRUM) context;

    float_t intensity0;
    ISTATUS status = SpectrumSampleInline(sum_spectrum->spectrum0,
                                          wavelength,
                                          &intensity0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t intensity1;
    status = SpectrumSampleInline(sum_spectrum->spectrum1,
                                  wavelength,
                                  &intensity1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *intensity = intensity0 + intensity1;
    return ISTATUS_SUCCESS; 
}

static
ISTATUS
AttenuatedReflectionSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(intensity != NULL);

    PCATTENUATED_REFLECTION_SPECTRUM spectrum = 
        (PCATTENUATED_REFLECTION_SPECTRUM) context;

    float_t spectrum_intensity;
    ISTATUS status = SpectrumSampleInline(spectrum->spectrum,
                                          wavelength,
                                          &spectrum_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflectInline(spectrum->reflector,
                                    wavelength,
                                    spectrum_intensity,
                                    &spectrum_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *intensity = spectrum_intensity * spectrum->attenuation;

    return ISTATUS_SUCCESS; 
}

//
// Static Variables
//

const static SPECTRUM_VTABLE sum_spectrum_vtable = {
    SumSpectrumSample,
    NULL
};

const static SPECTRUM_VTABLE attenuated_spectrum_vtable = {
    AttenuatedSpectrumSample,
    NULL
};

const static SPECTRUM_VTABLE attenuated_reflection_spectrum_vtable = {
    AttenuatedReflectionSpectrumSample,
    NULL
};

//
// Initialization Functions
//

static
inline
ISTATUS
AttenuatedSpectrumAllocate(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_ PCSPECTRUM spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenuated_spectrum
    )
{
    assert(compositor != NULL);
    assert(spectrum != NULL);
    assert(isfinite(attenuation));
    assert((float_t)0.0 < attenuation);
    assert(attenuated_spectrum != NULL);

    if (attenuation == (float_t)1.0)
    {
        *attenuated_spectrum = spectrum;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_SPECTRUM allocated_spectrum = (PATTENUATED_SPECTRUM) allocation;

    SpectrumInitialize(&allocated_spectrum->header,
                       &attenuated_spectrum_vtable,
                       allocated_spectrum);

    allocated_spectrum->spectrum = spectrum;
    allocated_spectrum->attenuation = attenuation;

    *attenuated_spectrum = &allocated_spectrum->header;

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
AttenuatedReflectionSpectrumAllocate(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenutated_reflection_spectrum
    )
{
    assert(compositor != NULL);
    assert(spectrum != NULL);
    assert(reflector != NULL);
    assert(isfinite(attenuation));
    assert((float_t)0.0 < attenuation);
    assert(attenutated_reflection_spectrum != NULL);

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_reflection_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_REFLECTION_SPECTRUM allocated_spectrum =
        (PATTENUATED_REFLECTION_SPECTRUM) allocation;

    SpectrumInitialize(&allocated_spectrum->header,
                       &attenuated_reflection_spectrum_vtable,
                       allocated_spectrum);

    allocated_spectrum->spectrum = spectrum;
    allocated_spectrum->reflector = reflector;
    allocated_spectrum->attenuation = attenuation;

    *attenutated_reflection_spectrum = &allocated_spectrum->header;

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
SpectrumCompositorAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (sum_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum0 == NULL)
    {
        *sum_spectrum = spectrum1;
        return ISTATUS_SUCCESS;
    }

    if (spectrum1 == NULL)
    {
        *sum_spectrum = spectrum0;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator,
        &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM)allocation;

    SpectrumInitialize(&allocated_spectrum->header,
                       &sum_spectrum_vtable,
                       allocated_spectrum);
    allocated_spectrum->spectrum0 = spectrum0;
    allocated_spectrum->spectrum1 = spectrum1;

    *sum_spectrum = &allocated_spectrum->header;

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_ PCSPECTRUM spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenuated_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if(!isfinite(attenuation) || attenuation <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (attenuated_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum->vtable == &attenuated_spectrum_vtable)
    {
        PCATTENUATED_SPECTRUM spectrum0 =
            (PCATTENUATED_SPECTRUM) spectrum;

        ISTATUS status =
            AttenuatedSpectrumAllocate(compositor,
                                       spectrum0->spectrum,
                                       attenuation * spectrum0->attenuation,
                                       attenuated_spectrum);

        return status;
    }

    if (spectrum->vtable == &attenuated_reflection_spectrum_vtable)
    {
        PCATTENUATED_REFLECTION_SPECTRUM spectrum0 =
            (PCATTENUATED_REFLECTION_SPECTRUM) spectrum;

        ISTATUS status =
            AttenuatedReflectionSpectrumAllocate(compositor,
                                                 spectrum0->spectrum,
                                                 spectrum0->reflector,
                                                 attenuation * spectrum0->attenuation,
                                                 attenuated_spectrum);

        return status;
    }

    ISTATUS status = AttenuatedSpectrumAllocate(compositor,
                                                spectrum,
                                                attenuation,
                                                attenuated_spectrum);

    return status;
}

ISTATUS
SpectrumCompositorAttenuateReflection(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_ PCSPECTRUM spectrum,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *reflected_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if(!isfinite(attenuation) || attenuation <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflected_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (spectrum == NULL)
    {
        *reflected_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = AttenuatedReflectionSpectrumAllocate(compositor,
                                                          spectrum,
                                                          reflector,
                                                          attenuation,
                                                          reflected_spectrum);

    return status;
}