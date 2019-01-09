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
// Macros
//

#define SPECTRUM_TYPE_ATTENUATED            0
#define SPECTRUM_TYPE_SUM                   1
#define SPECTRUM_TYPE_REFLECTION            2
#define SPECTRUM_TYPE_ATTENUATED_REFLECTION 3
#define SPECTRUM_TYPE_EXTERNAL              4

//
// Types
//

typedef
ISTATUS
(*PSPECTRUM_COMPOSITOR_ADD_SPECTRA)(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    );

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
ReflectionSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(intensity != NULL);

    PCREFLECTION_SPECTRUM reflection_spectrum = (PCREFLECTION_SPECTRUM) context;

    float_t spectrum_intensity;
    ISTATUS status = SpectrumSampleInline(reflection_spectrum->spectrum,
                                          wavelength,
                                          &spectrum_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflectInline(reflection_spectrum->reflector,
                                    wavelength,
                                    spectrum_intensity,
                                    intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

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

const static SPECTRUM_VTABLE vtables[] = {
    { AttenuatedSpectrumSample, NULL },
    { SumSpectrumSample, NULL },
    { ReflectionSpectrumSample, NULL },
    { AttenuatedReflectionSpectrumSample, NULL }
};

//
// Initialization Functions
//

static
void
AttenuatedSpectrumInitialize(
    _Out_ PATTENUATED_SPECTRUM attenutated_spectrum,
    _In_ PCSPECTRUM spectrum,
    _In_ float_t attenuation
    )
{
    assert(attenutated_spectrum != NULL);
    assert(spectrum != NULL);
    assert(isfinite(attenuation));
    assert((float_t)0.0 < attenuation);

    SpectrumInitialize(&vtables[SPECTRUM_TYPE_ATTENUATED],
                       attenutated_spectrum,
                       &attenutated_spectrum->header);
                                
    attenutated_spectrum->spectrum = spectrum;
    attenutated_spectrum->attenuation = attenuation;
}

static
void
SumSpectrumInitialize(
    _Out_ PSUM_SPECTRUM sum_spectrum,
    _In_ PCSPECTRUM spectrum0,
    _In_ PCSPECTRUM spectrum1
    )
{
    assert(sum_spectrum != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum1 != NULL);

    SpectrumInitialize(&vtables[SPECTRUM_TYPE_SUM],
                       sum_spectrum,
                       &sum_spectrum->header);
                                
    sum_spectrum->spectrum0 = spectrum0;
    sum_spectrum->spectrum1 = spectrum1;
}

static
void
ReflectionSpectrumInitialize(
    _Out_ PREFLECTION_SPECTRUM reflection_spectrum,
    _In_opt_ PCSPECTRUM spectrum,
    _In_ PCREFLECTOR reflector
    )
{
    assert(reflection_spectrum != NULL);
    assert(reflector != NULL);

    SpectrumInitialize(&vtables[SPECTRUM_TYPE_REFLECTION],
                       reflection_spectrum,
                       &reflection_spectrum->header);
                                
    reflection_spectrum->spectrum = spectrum;
    reflection_spectrum->reflector = reflector;
}

static
void
AttenuatedReflectionSpectrumInitialize(
    _Out_ PATTENUATED_REFLECTION_SPECTRUM AttenuatedReflectionSpectrum,
    _In_opt_ PCSPECTRUM spectrum,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation
    )
{
    assert(AttenuatedReflectionSpectrum != NULL);
    assert(reflector != NULL);
    assert(isfinite(attenuation));
    assert((float_t)0.0 < attenuation);

    SpectrumInitialize(&vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION],
                       AttenuatedReflectionSpectrum,
                       &AttenuatedReflectionSpectrum->header);
                                
    AttenuatedReflectionSpectrum->spectrum = spectrum;
    AttenuatedReflectionSpectrum->reflector = reflector;
    AttenuatedReflectionSpectrum->attenuation = attenuation;
}

//
// sum Functions
//

static
ISTATUS
SpectrumCompositorAddAttenuatedAndAttenuatedReflectionSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION]);
    assert(sum != NULL);

    PCATTENUATED_SPECTRUM attenuated_spectrum =
        (PCATTENUATED_SPECTRUM) spectrum0;
    PCATTENUATED_REFLECTION_SPECTRUM attenuated_reflection_spectrum = 
        (PCATTENUATED_REFLECTION_SPECTRUM) spectrum1;

    if (attenuated_spectrum->attenuation == attenuated_reflection_spectrum->attenuation)
    {
        PCSPECTRUM intermediate_spectrum;
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         attenuated_reflection_spectrum->spectrum,
                                                         attenuated_reflection_spectrum->reflector,
                                                         &intermediate_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              attenuated_spectrum->spectrum,
                                              intermediate_spectrum,
                                              &intermediate_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     intermediate_spectrum,
                                                     attenuated_spectrum->attenuation,
                                                     sum);

        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(&compositor->sum_spectrum_allocator, 
                                                 &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM sum_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(sum_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &sum_spectrum->header;
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumCompositorAddAttenuatedReflectionAndAttenuatedSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED]);
    assert(sum != NULL);

    ISTATUS status = 
        SpectrumCompositorAddAttenuatedAndAttenuatedReflectionSpectra(compositor,
                                                                      spectrum1,
                                                                      spectrum0,
                                                                      sum);

    return status;
}

static
ISTATUS
SpectrumCompositorAddTwoAttenuatedReflectionSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION]);
    assert(sum != NULL);

    PCATTENUATED_REFLECTION_SPECTRUM attenuated_reflection_spectrum0 = 
        (PCATTENUATED_REFLECTION_SPECTRUM) spectrum0;
    PCATTENUATED_REFLECTION_SPECTRUM attenuated_reflection_spectrum1 = 
        (PCATTENUATED_REFLECTION_SPECTRUM) spectrum1;

    if (attenuated_reflection_spectrum0->spectrum == attenuated_reflection_spectrum1->spectrum &&
        attenuated_reflection_spectrum0->reflector == attenuated_reflection_spectrum1->reflector)
    {
        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   attenuated_reflection_spectrum0->spectrum,
                                                                   attenuated_reflection_spectrum0->reflector,
                                                                   attenuated_reflection_spectrum0->attenuation + attenuated_reflection_spectrum1->attenuation,
                                                                   sum);
        return status;
    }

    if (attenuated_reflection_spectrum0->attenuation == attenuated_reflection_spectrum1->attenuation)
    {
        PCSPECTRUM intermediate_spectrum0;
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         attenuated_reflection_spectrum0->spectrum,
                                                         attenuated_reflection_spectrum0->reflector,
                                                         &intermediate_spectrum0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        PCSPECTRUM intermediate_spectrum1;
        status = SpectrumCompositorAddReflection(compositor,
                                                 attenuated_reflection_spectrum1->spectrum,
                                                 attenuated_reflection_spectrum1->reflector,
                                                 &intermediate_spectrum1);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
        
        status = SpectrumCompositorAddSpectra(compositor,
                                              intermediate_spectrum0,
                                              intermediate_spectrum1,
                                              &intermediate_spectrum0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     intermediate_spectrum0,
                                                     attenuated_reflection_spectrum0->attenuation,
                                                     sum);
        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(allocated_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumCompositorAddTwoReflectionSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_REFLECTION]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_REFLECTION]);
    assert(sum != NULL);

    PCREFLECTION_SPECTRUM reflection_spectrum0 = 
        (PCREFLECTION_SPECTRUM) spectrum0;
    PCREFLECTION_SPECTRUM reflection_spectrum1 = 
        (PCREFLECTION_SPECTRUM) spectrum1;

    if (reflection_spectrum0->spectrum == reflection_spectrum1->spectrum &&
        reflection_spectrum0->reflector == reflection_spectrum1->reflector)
    {
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             spectrum0,
                                                             (float_t)2.0,
                                                             sum);
        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(allocated_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumCompositorAddTwoAttenuatedSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED]);
    assert(sum != NULL);

    PCATTENUATED_SPECTRUM attenuated_spectrum0 = 
        (PCATTENUATED_SPECTRUM) spectrum0;
    PCATTENUATED_SPECTRUM attenuated_spectrum1 = 
        (PCATTENUATED_SPECTRUM) spectrum1;

    if (attenuated_spectrum0->spectrum == attenuated_spectrum1->spectrum)
    {
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             attenuated_spectrum0->spectrum,
                                                             attenuated_spectrum0->attenuation + attenuated_spectrum1->attenuation,
                                                             sum);
        return status;
    }

    if (attenuated_spectrum0->attenuation == attenuated_spectrum1->attenuation)
    {
        PCSPECTRUM intermediate_spectrum;
        ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                      attenuated_spectrum0->spectrum,
                                                      attenuated_spectrum1->spectrum,
                                                      &intermediate_spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     intermediate_spectrum,
                                                     attenuated_spectrum0->attenuation,
                                                     sum);
        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(allocated_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumCompositorAddTwoSumSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
    assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum0->vtable == &vtables[SPECTRUM_TYPE_SUM]);
    assert(spectrum1 != NULL);
    assert(spectrum1->vtable == &vtables[SPECTRUM_TYPE_SUM]);
    assert(sum != NULL);

    PCSUM_SPECTRUM sum_spectrum0 = (PCSUM_SPECTRUM) spectrum0;
    PCSUM_SPECTRUM sum_spectrum1 = (PCSUM_SPECTRUM) spectrum1;

    if (sum_spectrum0->spectrum0 == sum_spectrum1->spectrum0)
    {
        if (sum_spectrum0->spectrum1 == sum_spectrum1->spectrum1)
        {
            ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                                 spectrum0,
                                                                 (float_t)2.0,
                                                                 sum);
            return status;
        }
        else
        {
            PCSPECTRUM intermediate_spectrum0;
            ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                                 sum_spectrum0->spectrum0,
                                                                 (float_t)2.0,
                                                                 &intermediate_spectrum0);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            PCSPECTRUM intermediate_spectrum1;
            status = SpectrumCompositorAddSpectra(compositor,
                                                  sum_spectrum0->spectrum1,
                                                  sum_spectrum1->spectrum1,
                                                  &intermediate_spectrum1);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            status = SpectrumCompositorAddSpectra(compositor,
                                                  intermediate_spectrum0,
                                                  intermediate_spectrum1,
                                                  sum);
            return status;
        }
    }
    else if (sum_spectrum0->spectrum1 == sum_spectrum1->spectrum0)
    {
        if (sum_spectrum0->spectrum0 == sum_spectrum1->spectrum1)
        {
            ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                                 spectrum0,
                                                                 (float_t)2.0,
                                                                 sum);
            return status;
        }
        else
        {
            PCSPECTRUM intermediate_spectrum0;
            ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                                 sum_spectrum0->spectrum1,
                                                                 (float_t)2.0,
                                                                 &intermediate_spectrum0);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            PCSPECTRUM intermediate_spectrum1;
            status = SpectrumCompositorAddSpectra(compositor,
                                                  sum_spectrum0->spectrum0,
                                                  sum_spectrum1->spectrum1,
                                                  &intermediate_spectrum1);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            status = SpectrumCompositorAddSpectra(compositor,
                                                  intermediate_spectrum0,
                                                  intermediate_spectrum1,
                                                  sum);
            return status;
        }
    }
    else if (sum_spectrum0->spectrum1 == sum_spectrum1->spectrum1)
    {
        PCSPECTRUM intermediate_spectrum0;
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             sum_spectrum0->spectrum1,
                                                             (float_t)2.0,
                                                             &intermediate_spectrum0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        PCSPECTRUM intermediate_spectrum1;
        status = SpectrumCompositorAddSpectra(compositor,
                                              sum_spectrum0->spectrum0,
                                              sum_spectrum1->spectrum0,
                                              &intermediate_spectrum1);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              intermediate_spectrum0,
                                              intermediate_spectrum1,
                                              sum);
        return status;
    }
    else if (sum_spectrum0->spectrum0 == sum_spectrum1->spectrum1)
    {
        PCSPECTRUM intermediate_spectrum0;
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             sum_spectrum0->spectrum0,
                                                             (float_t)2.0,
                                                             &intermediate_spectrum0);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        PCSPECTRUM intermediate_spectrum1;
        status = SpectrumCompositorAddSpectra(compositor,
                                              sum_spectrum0->spectrum1,
                                              sum_spectrum1->spectrum0,
                                              &intermediate_spectrum1);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        status = SpectrumCompositorAddSpectra(compositor,
                                              intermediate_spectrum0,
                                              intermediate_spectrum1,
                                              sum);
        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(allocated_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumCompositorAddTwoSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum0,
    _In_opt_ PCSPECTRUM spectrum1,
    _Out_ PCSPECTRUM *sum
    )
{
	assert(compositor != NULL);
    assert(spectrum0 != NULL);
    assert(spectrum1 != NULL);
    assert(sum != NULL);

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->sum_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSUM_SPECTRUM allocated_spectrum = (PSUM_SPECTRUM) allocation;

    SumSpectrumInitialize(allocated_spectrum,
                          spectrum0,
                          spectrum1);

    *sum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

const static PSPECTRUM_COMPOSITOR_ADD_SPECTRA AddSpectrumRoutines[5][5] = {
    { SpectrumCompositorAddTwoAttenuatedSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddAttenuatedAndAttenuatedReflectionSpectra, 
      SpectrumCompositorAddTwoSpectra },
    { SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSumSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra },
    { SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoReflectionSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra },
    { SpectrumCompositorAddAttenuatedReflectionAndAttenuatedSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoAttenuatedReflectionSpectra, 
      SpectrumCompositorAddTwoSpectra },
    { SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra, 
      SpectrumCompositorAddTwoSpectra }
};

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

    if (spectrum0 == spectrum1)
    {
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             spectrum0,
                                                             (float_t)2.0, 
                                                             sum_spectrum);

        return status;
    }

    size_t type0;
    if (&vtables[0] <= spectrum0->vtable &&
        spectrum0->vtable < &vtables[SPECTRUM_TYPE_EXTERNAL])
    {
        type0 = spectrum0->vtable - &vtables[0];
    }
    else
    {
        type0 = SPECTRUM_TYPE_EXTERNAL;
    }

    size_t type1;
    if (&vtables[0] <= spectrum1->vtable &&
        spectrum1->vtable < &vtables[SPECTRUM_TYPE_EXTERNAL])
    {
        type1 = spectrum1->vtable - &vtables[0];
    }
    else
    {
        type1 = SPECTRUM_TYPE_EXTERNAL;
    }

    ISTATUS status = AddSpectrumRoutines[type0][type1](compositor,
                                                       spectrum0,
                                                       spectrum1,
                                                       sum_spectrum);

    return status;
}

ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _In_opt_ PCSPECTRUM spectrum,
    _In_ float_t attenuation,
    _Out_ PCSPECTRUM *attenutated_spectrum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || (float_t)0.0 > attenuation)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (attenutated_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL || (float_t)0.0 == attenuation)
    {
        *attenutated_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (attenuation == (float_t)1.0)
    {
        *attenutated_spectrum = spectrum;
        return ISTATUS_SUCCESS;
    }

    if (spectrum->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED])
    {
        PCATTENUATED_SPECTRUM attenuated_spectrum0 = 
            (PCATTENUATED_SPECTRUM) spectrum;

        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             attenuated_spectrum0->spectrum,
                                                             attenuation * attenuated_spectrum0->attenuation,
                                                             attenutated_spectrum);

        return status;
    }

    if (spectrum->vtable == &vtables[SPECTRUM_TYPE_ATTENUATED_REFLECTION])
    {
        PCATTENUATED_REFLECTION_SPECTRUM attenuated_spectrum0 = 
            (PCATTENUATED_REFLECTION_SPECTRUM) spectrum;

        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   attenuated_spectrum0->spectrum,
                                                                   attenuated_spectrum0->reflector,
                                                                   attenuation * attenuated_spectrum0->attenuation,
                                                                   attenutated_spectrum);

        return status;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_SPECTRUM allocated_spectrum = (PATTENUATED_SPECTRUM) allocation;

    AttenuatedSpectrumInitialize(allocated_spectrum,
                                 spectrum,
                                 attenuation);

    *attenutated_spectrum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumCompositorAddReflection(
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

    if (spectrum == NULL || reflector == NULL)
    {
        *reflected_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->reflection_spectrum_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PREFLECTION_SPECTRUM allocated_spectrum = (PREFLECTION_SPECTRUM) allocation;

    ReflectionSpectrumInitialize(allocated_spectrum,
                                 spectrum,
                                 reflector);

    *reflected_spectrum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumCompositorAttenuatedAddReflection(
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

    if(!isfinite(attenuation) || (float_t)0.0 > attenuation)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflected_spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (spectrum == NULL || reflector == NULL || (float_t)0.0 == attenuation)
    {
        *reflected_spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (attenuation == (float_t)1.0f)
    {
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         spectrum,
                                                         reflector,
                                                         reflected_spectrum);

        return status;
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

    AttenuatedReflectionSpectrumInitialize(allocated_spectrum,
                                           spectrum,
                                           reflector,
                                           attenuation);

    *reflected_spectrum = &allocated_spectrum->header;
    return ISTATUS_SUCCESS;
}