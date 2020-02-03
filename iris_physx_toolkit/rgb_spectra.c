/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    rgb_spectra.c

Abstract:

    Implements the RGB spectrum, reflector, color integrator, and color
    extrapolator.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/rgb_spectra.h"

//
// Constants
//

#define R_WAVELENGTH ((float_t)0.5)
#define G_WAVELENGTH ((float_t)1.5)
#define B_WAVELENGTH ((float_t)2.5)

//
// Types
//

typedef struct _RGB_SPECTRUM {
    float_t data[3];
} RGB_SPECTRUM, *PRGB_SPECTRUM;

typedef const RGB_SPECTRUM *PCRGB_SPECTRUM;

typedef struct _RGB_REFLECTOR {
    float_t data[3];
    float_t albedo;
} RGB_REFLECTOR, *PRGB_REFLECTOR;

typedef const RGB_REFLECTOR *PCRGB_REFLECTOR;

//
// Static Functions
//

static
inline
COLOR3
RgbToXyz(
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b
    )
{
  float_t x = (float_t)0.412453 * r +
              (float_t)0.357580 * g +
              (float_t)0.180423 * b;
  float_t y = (float_t)0.212671 * r +
              (float_t)0.715160 * g +
              (float_t)0.072169 * b;
  float_t z = (float_t)0.019334 * r +
              (float_t)0.119193 * g +
              (float_t)0.950227 * b;

  x = fmax((float_t)0.0, x);
  y = fmax((float_t)0.0, y);
  z = fmax((float_t)0.0, z);

  return ColorCreate(x, y, z);
}

static
ISTATUS
RgbSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCRGB_SPECTRUM rgb_spectrum = (PCRGB_SPECTRUM)context;

    if (B_WAVELENGTH < wavelength)
    {
        *intensity = (float_t)0.0;
    }
    else
    {
        *intensity = rgb_spectrum->data[(size_t)wavelength];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
RgbReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCRGB_REFLECTOR rgb_reflector = (PCRGB_REFLECTOR)context;

    if (B_WAVELENGTH < wavelength)
    {
        *reflectance = (float_t)0.0;
    }
    else
    {
        *reflectance = rgb_reflector->data[(size_t)wavelength];
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
RgbReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCRGB_REFLECTOR rgb_reflector = (PCRGB_REFLECTOR)context;

    *albedo = rgb_reflector->albedo;

    return ISTATUS_SUCCESS;
}

ISTATUS
RgbColorIntegratorComputeSpectrumColor(
    _In_ const void *context,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    float_t r;
    ISTATUS status = SpectrumSample(spectrum,
                                    R_WAVELENGTH,
                                    &r);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t g;
    status = SpectrumSample(spectrum,
                            G_WAVELENGTH,
                            &g);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t b;
    status = SpectrumSample(spectrum,
                            B_WAVELENGTH,
                            &b);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *color = RgbToXyz(r, g, b);

    return ISTATUS_SUCCESS;
}

ISTATUS
RgbColorIntegratorComputeReflectorColor(
    _In_ const void *context,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    float_t r;
    ISTATUS status = ReflectorReflect(reflector,
                                      R_WAVELENGTH,
                                      &r);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t g;
    status = ReflectorReflect(reflector,
                              G_WAVELENGTH,
                              &g);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t b;
    status = ReflectorReflect(reflector,
                              B_WAVELENGTH,
                              &b);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *color = RgbToXyz(r, g, b);
    color->x = fmin(color->x, (float_t)1.0);
    color->y = fmin(color->y, (float_t)1.0);
    color->z = fmin(color->z, (float_t)1.0);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const SPECTRUM_VTABLE rgb_spectrum_vtable = {
    RgbSpectrumSample,
    NULL
};

static const REFLECTOR_VTABLE rgb_reflector_vtable = {
    RgbReflectorReflect,
    RgbReflectorGetAlbedo,
    NULL
};

//
// Static Functions
//

static
ISTATUS
RgbColorExtrapolatorComputeSpectrum(
    _In_ const void *context,
    _In_ const float_t color[3],
    _Out_ PSPECTRUM *spectrum
    )
{
    if (!isfinite(color[0]) || color[0] < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(color[1]) || color[1] < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(color[2]) || color[2] < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    RGB_SPECTRUM rgb_spectrum;
    rgb_spectrum.data[0] = color[0];
    rgb_spectrum.data[1] = color[1];
    rgb_spectrum.data[2] = color[2];

    ISTATUS status = SpectrumAllocate(&rgb_spectrum_vtable,
                                      &rgb_spectrum,
                                      sizeof(RGB_SPECTRUM),
                                      alignof(RGB_SPECTRUM),
                                      spectrum);

    return status;
}

static
ISTATUS
RgbColorExtrapolatorComputeReflector(
    _In_ const void *context,
    _In_ const float_t color[3],
    _Out_ PREFLECTOR *reflector
    )
{
    if (!isfinite(color[0]) ||
        color[0] < (float_t)0.0 ||
        (float_t)1.0 < color[0])
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(color[1]) ||
        color[1] < (float_t)0.0 ||
        (float_t)1.0 < color[1])
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(color[2]) ||
        color[2] < (float_t)0.0 ||
        (float_t)1.0 < color[2])
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    COLOR3 xyz = RgbToXyz(color[0], color[1], color[2]);

    RGB_REFLECTOR rgb_reflector;
    rgb_reflector.data[0] = color[0];
    rgb_reflector.data[1] = color[1];
    rgb_reflector.data[2] = color[2];
    rgb_reflector.albedo = fmin(xyz.y, (float_t)1.0);

    ISTATUS status = ReflectorAllocate(&rgb_reflector_vtable,
                                       &rgb_reflector,
                                       sizeof(RGB_REFLECTOR),
                                       alignof(RGB_REFLECTOR),
                                       reflector);

    return status;
}

//
// Static Variables
//

static const COLOR_INTEGRATOR_VTABLE rgb_integrator_vtable = {
    RgbColorIntegratorComputeSpectrumColor,
    RgbColorIntegratorComputeReflectorColor,
    NULL
};

static const COLOR_EXTRAPOLATOR_VTABLE rgb_extrapolator_vtable = {
    RgbColorExtrapolatorComputeSpectrum,
    RgbColorExtrapolatorComputeReflector,
    NULL
};

//
// Functions
//

ISTATUS
RgbColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = ColorIntegratorAllocate(&rgb_integrator_vtable,
                                             NULL,
                                             0,
                                             0,
                                             color_integrator);

    return status;
}

ISTATUS
RgbColorExtrapolatorAllocate(
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = ColorExtrapolatorAllocate(&rgb_extrapolator_vtable,
                                               NULL,
                                               0,
                                               0,
                                               color_extrapolator);

    return status;
}