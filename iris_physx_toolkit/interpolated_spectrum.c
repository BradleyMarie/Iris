/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    interpolated_spectrum.c

Abstract:

    Implements an interpolated spectrum.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/interpolated_spectrum.h"

//
// Types
//

typedef struct _INTERPOLATED_SPECTRUM {
    _Field_size_(num_samples) float_t *wavelengths;
    _Field_size_(num_samples) float_t *intensities;
    size_t num_samples;
} INTERPOLATED_SPECTRUM, *PINTERPOLATED_SPECTRUM;

typedef const INTERPOLATED_SPECTRUM *PCINTERPOLATED_SPECTRUM;

//
// Static Functions
//

static
inline
float_t
Interpolate(
    _In_reads_(num_samples) const float_t *wavelengths,
    _In_reads_(num_samples) const float_t *values,
    _In_ size_t num_samples,
    _In_ float_t wavelength
    )
{
    assert(wavelengths != NULL);
    assert(values != NULL);
    assert(num_samples != 0);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);

    const float_t *lower_bound = wavelengths;
    size_t num_wavelengths = num_samples;

    while (num_wavelengths > 0)
    {
        size_t step = num_wavelengths >> 1;
        const float_t *pivot = lower_bound + step;
        if (*pivot < wavelength)
        {
            lower_bound = pivot + 1;
            num_wavelengths -= step + 1;
        }
        else
        {
            num_wavelengths = step;
        }
    }

    size_t result_index = lower_bound - wavelengths;

    if (result_index == num_samples)
    {
        return (float_t)0.0;
    }

    float_t higher_wavelength = *lower_bound;

    if (higher_wavelength == wavelength)
    {
        return values[result_index];
    }

    if (result_index == 0)
    {
        return (float_t)0.0;
    }

    float_t lower_wavelength = lower_bound[-1];
    float_t parameter =
        (wavelength - lower_wavelength) / 
        (higher_wavelength - lower_wavelength);

    float_t higher_value = values[result_index];
    float_t lower_value = values[result_index - 1];
    return fma(parameter, higher_value - lower_value, lower_value);
}

static
ISTATUS
InterpolatedSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCINTERPOLATED_SPECTRUM interpolated_spectrum =
        (PCINTERPOLATED_SPECTRUM)context;

    *intensity = Interpolate(interpolated_spectrum->wavelengths,
                             interpolated_spectrum->intensities,
                             interpolated_spectrum->num_samples,
                             wavelength);

    return ISTATUS_SUCCESS;
}

static
void
InterpolatedSpectrumFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PINTERPOLATED_SPECTRUM interpolated_spectrum =
        (PINTERPOLATED_SPECTRUM)context;

    free(interpolated_spectrum->wavelengths);
    free(interpolated_spectrum->intensities);
}

//
// Static Variables
//

static const SPECTRUM_VTABLE interpolated_spectrum_vtable = {
    InterpolatedSpectrumSample,
    InterpolatedSpectrumFree
};

//
// Functions
//

ISTATUS
InterpolatedSpectrumAllocate(
    _In_reads_(num_samples) const float_t *wavelengths,
    _In_reads_(num_samples) const float_t *intensities,
    _In_ size_t num_samples,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (wavelengths == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (intensities == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (num_samples < 2)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    INTERPOLATED_SPECTRUM interpolated_spectrum;
    interpolated_spectrum.wavelengths = calloc(num_samples, sizeof(float_t));

    if (interpolated_spectrum.wavelengths == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    interpolated_spectrum.intensities = calloc(num_samples, sizeof(float_t));

    if (interpolated_spectrum.intensities == NULL)
    {
        free(interpolated_spectrum.wavelengths);
        return ISTATUS_ALLOCATION_FAILED;
    }

    float_t last_wavelength = (float_t)0.0;
    for (size_t i = 0; i < num_samples; i++)
    {
        if (!isfinite(wavelengths[i]) ||
            wavelengths[i] <= (float_t)0.0 ||
            wavelengths[i] < last_wavelength)
        {
            free(interpolated_spectrum.wavelengths);
            free(interpolated_spectrum.intensities);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        interpolated_spectrum.wavelengths[i] = wavelengths[i];
        last_wavelength = wavelengths[i];

        if (!isfinite(intensities[i]) || intensities[i] < (float_t)0.0)
        {
            free(interpolated_spectrum.wavelengths);
            free(interpolated_spectrum.intensities);
            return ISTATUS_INVALID_ARGUMENT_01;
        }

        interpolated_spectrum.intensities[i] = intensities[i];
    }

    interpolated_spectrum.num_samples = num_samples;

    ISTATUS status = SpectrumAllocate(&interpolated_spectrum_vtable,
                                      &interpolated_spectrum,
                                      sizeof(INTERPOLATED_SPECTRUM),
                                      alignof(INTERPOLATED_SPECTRUM),
                                      spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        free(interpolated_spectrum.wavelengths);
        free(interpolated_spectrum.intensities);
    }

    return status;
}