/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    metric_black_body.c

Abstract:

    Implements a metric black body.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/metric_black_body.h"

//
// Types
//

typedef struct _METRIC_BLACK_BODY {
    float_t temperature;
} METRIC_BLACK_BODY, *PMETRIC_BLACKBODY;

typedef const METRIC_BLACK_BODY *PCMETRIC_BLACK_BODY;

//
// Static Functions
//

// Assumes
// speed of light (c) = 299792458 m / s
// planck's constant (h) = 6.62607004081e-34 J / s
// boltzmann constant (kb) = 1.3806485279e-23 J / K
static const float_t two_h_c_squared = (float_t)1.1910429527701728208248655e-16;
static const float_t hc_over_kb = (float_t)0.0143877734577099259077839632410620;

static
ISTATUS
MetricBlackBodySample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCMETRIC_BLACK_BODY metric_black_body = (PCMETRIC_BLACK_BODY)context;

    float_t wavelength_in_meters = wavelength * 1e-9;
    float_t wavelength_to_the_fifth = pow(wavelength_in_meters, (float_t)5.0);
    float_t exponent = 
        hc_over_kb / (wavelength_in_meters * metric_black_body->temperature);

    *intensity = two_h_c_squared / 
        (wavelength_to_the_fifth * exp(exponent) - (float_t)1.0);
    assert(!isnan(*intensity));

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const SPECTRUM_VTABLE metric_black_body_vtable = {
    MetricBlackBodySample,
    NULL
};

//
// Functions
//

ISTATUS
MetricBlackBodyAllocate(
    _In_ float_t temperature,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (!isfinite(temperature)|| temperature <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    METRIC_BLACK_BODY metric_black_body;
    metric_black_body.temperature = temperature;

    ISTATUS status = SpectrumAllocate(&metric_black_body_vtable,
                                      &metric_black_body,
                                      sizeof(METRIC_BLACK_BODY),
                                      alignof(METRIC_BLACK_BODY),
                                      spectrum);

    return status;
}