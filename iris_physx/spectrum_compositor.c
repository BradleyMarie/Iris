/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum_compositor.c

Abstract:

    Enables the composition of spectra and reflectors into new spectra.

--*/

#include "iris_physx/spectrum_compositor.h"

#include <assert.h>

#include "iris_physx/reflector_compositor_internal.h"
#include "iris_physx/reflector_internal.h"
#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/spectrum_internal.h"

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
AttenuatedSumSpectrumReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCATTENUATED_SUM_SPECTRUM spectrum = (PCATTENUATED_SUM_SPECTRUM)context;

    float_t added_intensity;
    ISTATUS status = SpectrumSampleInline(spectrum->added_spectrum,
                                          wavelength,
                                          &added_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t attenuated_intensity;
    status = SpectrumSampleInline(spectrum->attenuated_spectrum,
                                  wavelength,
                                  &attenuated_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *intensity =
        added_intensity + attenuated_intensity * spectrum->attenuation;

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

    float_t reflectance;
    status = ReflectorReflectInline(spectrum->reflector,
                                    wavelength,
                                    &reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *intensity = spectrum_intensity * reflectance * spectrum->attenuation;

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

const static SPECTRUM_VTABLE attenuated_sum_spectrum_vtable = {
    AttenuatedSumSpectrumReflect,
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
    assert((float_t)0.0 <= attenuation);
    assert(attenuated_spectrum != NULL);

    if (attenuation == (float_t)0.0)
    {
        *attenuated_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

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

    InternalSpectrumInitialize(&allocated_spectrum->header,
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
    assert((float_t)0.0 <= attenuation);
    assert(attenutated_reflection_spectrum != NULL);

    if (attenuation == (float_t)0.0)
    {
        *attenutated_reflection_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_reflection_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_REFLECTION_SPECTRUM allocated_spectrum =
        (PATTENUATED_REFLECTION_SPECTRUM) allocation;

    InternalSpectrumInitialize(&allocated_spectrum->header,
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

    InternalSpectrumInitialize(&allocated_spectrum->header,
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
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenuated_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || attenuation < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (attenuated_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL)
    {
        *attenuated_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (spectrum->vtable == (const void*)&attenuated_reflection_spectrum_vtable)
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

    if (spectrum->vtable == (const void*)&attenuated_spectrum_vtable)
    {
        PCATTENUATED_SPECTRUM spectrum0 =
            (PCATTENUATED_SPECTRUM) spectrum;

        spectrum = spectrum0->spectrum;
        attenuation *= spectrum0->attenuation;
    }

    ISTATUS status = AttenuatedSpectrumAllocate(compositor,
                                                spectrum,
                                                attenuation,
                                                attenuated_spectrum);

    return status;
}

ISTATUS
SpectrumCompositorAttenuatedAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_ PCSPECTRUM added_spectrum,
    _In_ PCSPECTRUM attenuated_spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *result
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || attenuation < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (added_spectrum == NULL)
    {
        if (attenuated_spectrum == NULL)
        {
            *result = NULL;
            return ISTATUS_SUCCESS;
        }

        ISTATUS status = AttenuatedSpectrumAllocate(compositor,
                                                    attenuated_spectrum,
                                                    attenuation,
                                                    result);

        return status;
    }

    if (attenuated_spectrum == NULL)
    {
        *result = added_spectrum;
        return ISTATUS_SUCCESS;
    }

    if (attenuated_spectrum->vtable == (const void*)&attenuated_spectrum_vtable)
    {
        PCATTENUATED_SPECTRUM spectrum0 =
            (PCATTENUATED_SPECTRUM)attenuated_spectrum;

        attenuated_spectrum = spectrum0->spectrum;
        attenuation *= spectrum0->attenuation;
    }

    if (attenuation == (float_t)0.0)
    {
        *result = added_spectrum;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_SUM_SPECTRUM allocated_spectrum =
        (PATTENUATED_SUM_SPECTRUM)allocation;

    InternalSpectrumInitialize(&allocated_spectrum->header,
                               &attenuated_sum_spectrum_vtable,
                               allocated_spectrum);

    allocated_spectrum->added_spectrum = added_spectrum;
    allocated_spectrum->attenuated_spectrum = attenuated_spectrum;
    allocated_spectrum->attenuation = attenuation;

    *result = &allocated_spectrum->header;

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumCompositorAttenuateReflection(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *reflected_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || attenuation < (float_t)0.0)
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

    if (reflector == NULL)
    {
        *reflected_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (spectrum->vtable == (const void*)&attenuated_spectrum_vtable)
    {
        PCATTENUATED_SPECTRUM spectrum0 =
            (PCATTENUATED_SPECTRUM)spectrum;

        spectrum = spectrum0->spectrum;
        attenuation *= spectrum0->attenuation;
    }

    if (reflector->reference_count == ATTENUATED_REFLECTOR_TYPE)
    {
        PCATTENUATED_REFLECTOR reflector0 =
            (PCATTENUATED_REFLECTOR)reflector;

        reflector = reflector0->reflector;
        attenuation *= reflector0->attenuation;
    }

    ISTATUS status = AttenuatedReflectionSpectrumAllocate(compositor,
                                                          spectrum,
                                                          reflector,
                                                          attenuation,
                                                          reflected_spectrum);

    return status;
}

ISTATUS
SpectrumCompositorReflect(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCSPECTRUM *reflected_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflected_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    // TODO: Implement an actual reflected spectrum if this ends up being hot.
    ISTATUS status = SpectrumCompositorAttenuateReflection(compositor,
                                                           spectrum,
                                                           reflector,
                                                           (float_t)1.0,
                                                           reflected_spectrum);

    return status;
}