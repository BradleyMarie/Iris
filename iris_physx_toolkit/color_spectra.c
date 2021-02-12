/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_spectra.c

Abstract:

    Implements the COLOR spectrum, reflector, color integrator, and color
    extrapolator.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/color_spectra.h"

//
// Constants
//

#define FIRST_WAVELENGTH ((float_t)0.5)
#define SECOND_WAVELENGTH ((float_t)1.5)
#define THIRD_WAVELENGTH ((float_t)2.5)

//
// Types
//

typedef struct _COLOR_SPECTRUM {
    float_t values[3];
} COLOR_SPECTRUM, *PCOLOR_SPECTRUM;

typedef const COLOR_SPECTRUM *PCCOLOR_SPECTRUM;

typedef struct _COLOR_REFLECTOR {
    float_t values[3];
    float_t albedo;
} COLOR_REFLECTOR, *PCOLOR_REFLECTOR;

typedef const COLOR_REFLECTOR *PCCOLOR_REFLECTOR;

//
// Static Functions
//

static
ISTATUS
ColorSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCCOLOR_SPECTRUM rgb_spectrum = (PCCOLOR_SPECTRUM)context;

    if (THIRD_WAVELENGTH < wavelength)
    {
        *intensity = (float_t)0.0;
    }
    else
    {
        *intensity = rgb_spectrum->values[(size_t)wavelength];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ColorReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCCOLOR_REFLECTOR rgb_reflector = (PCCOLOR_REFLECTOR)context;

    if (THIRD_WAVELENGTH < wavelength)
    {
        *reflectance = (float_t)0.0;
    }
    else
    {
        *reflectance = rgb_reflector->values[(size_t)wavelength];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ColorReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCCOLOR_REFLECTOR rgb_reflector = (PCCOLOR_REFLECTOR)context;

    *albedo = rgb_reflector->albedo;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorColorIntegratorComputeSpectrumColor(
    _In_ const void *context,
    _In_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    PCCOLOR_SPACE color_space = (PCCOLOR_SPACE)context;

    float_t values[3];
    ISTATUS status = SpectrumSample(spectrum,
                                    FIRST_WAVELENGTH,
                                    values + 0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumSample(spectrum,
                            SECOND_WAVELENGTH,
                            values + 1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumSample(spectrum,
                            THIRD_WAVELENGTH,
                            values + 2);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *color = ColorCreate(*color_space, values);

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorColorIntegratorComputeReflectorColor(
    _In_ const void *context,
    _In_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    PCCOLOR_SPACE color_space = (PCCOLOR_SPACE)context;

    float_t values[3];
    ISTATUS status = ReflectorReflect(reflector,
                                      FIRST_WAVELENGTH,
                                      values + 0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflect(reflector,
                              SECOND_WAVELENGTH,
                              values + 1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflect(reflector,
                              THIRD_WAVELENGTH,
                              values + 2);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *color = ColorCreate(*color_space, values);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const SPECTRUM_VTABLE color_spectrum_vtable = {
    ColorSpectrumSample,
    NULL
};

static const REFLECTOR_VTABLE color_reflector_vtable = {
    ColorReflectorReflect,
    ColorReflectorGetAlbedo,
    NULL
};

//
// Static Functions
//

static
ISTATUS
ColorColorExtrapolatorComputeSpectrum(
    _In_ const void *context,
    _In_ COLOR3 color,
    _Out_ PSPECTRUM *spectrum
    )
{
    PCCOLOR_SPACE color_space = (PCCOLOR_SPACE)context;

    color = ColorConvert(color, *color_space);

    COLOR_SPECTRUM color_spectrum;
    color_spectrum.values[0] = color.values[0];
    color_spectrum.values[1] = color.values[1];
    color_spectrum.values[2] = color.values[2];

    ISTATUS status = SpectrumAllocate(&color_spectrum_vtable,
                                      &color_spectrum,
                                      sizeof(COLOR_SPECTRUM),
                                      alignof(COLOR_SPECTRUM),
                                      spectrum);

    return status;
}

static
ISTATUS
ColorColorExtrapolatorComputeReflector(
    _In_ const void *context,
    _In_ COLOR3 color,
    _Out_ PREFLECTOR *reflector
    )
{
    PCCOLOR_SPACE color_space = (PCCOLOR_SPACE)context;

    color = ColorConvert(color, *color_space);
    color = ColorClamp(color, 1.0f);

    COLOR_REFLECTOR color_reflector;
    color_reflector.values[0] = color.values[0];
    color_reflector.values[1] = color.values[1];
    color_reflector.values[2] = color.values[2];
    color_reflector.albedo = ColorToClampedLuma(color, (float_t)1.0f);

    ISTATUS status = ReflectorAllocate(&color_reflector_vtable,
                                       &color_reflector,
                                       sizeof(COLOR_REFLECTOR),
                                       alignof(COLOR_REFLECTOR),
                                       reflector);

    return status;
}

//
// Static Variables
//

static const COLOR_INTEGRATOR_VTABLE color_color_integrator_vtable = {
    ColorColorIntegratorComputeSpectrumColor,
    ColorColorIntegratorComputeReflectorColor,
    NULL
};

static const COLOR_EXTRAPOLATOR_VTABLE color_color_extrapolator_vtable = {
    ColorColorExtrapolatorComputeSpectrum,
    ColorColorExtrapolatorComputeReflector,
    NULL
};

//
// Functions
//

ISTATUS
ColorColorIntegratorAllocate(
    _In_ COLOR_SPACE color_space,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = ColorIntegratorAllocate(&color_color_integrator_vtable,
                                             &color_space,
                                             sizeof(COLOR_SPACE),
                                             alignof(COLOR_SPACE),
                                             color_integrator);

    return status;
}

ISTATUS
ColorColorExtrapolatorAllocate(
    _In_ COLOR_SPACE color_space,
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = ColorExtrapolatorAllocate(&color_color_extrapolator_vtable,
                                               &color_space,
                                               sizeof(COLOR_SPACE),
                                               alignof(COLOR_SPACE),
                                               color_extrapolator);

    return status;
}