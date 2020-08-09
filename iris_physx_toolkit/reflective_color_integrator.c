/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    reflcetive_color_integrator.c

Abstract:

    Implements a CIE color integrator.

--*/

#include "iris_physx_toolkit/interpolated_spectrum.h"
#include "iris_physx_toolkit/reflective_color_integrator.h"

#include <stdalign.h>

//
// Types
//

typedef struct _REFLECTIVE_INTEGRATOR {
    PCOLOR_INTEGRATOR color_integrator;
    float_t spectral_to_reflective[2][3];
} REFLECTIVE_INTEGRATOR, *PREFLECTIVE_INTEGRATOR;

typedef const REFLECTIVE_INTEGRATOR *PCREFLECTIVE_INTEGRATOR;

//
// Static Functions
//

ISTATUS
ReflectiveColorIntegratorComputeSpectrumColor(
    _In_ const void *context,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    PCREFLECTIVE_INTEGRATOR integrator = (PCREFLECTIVE_INTEGRATOR)context;

    ISTATUS status =
        ColorIntegratorComputeSpectrumColor(integrator->color_integrator,
                                            spectrum,
                                            color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    color->values[0] *= integrator->spectral_to_reflective[color->color_space][0];
    color->values[1] *= integrator->spectral_to_reflective[color->color_space][1];
    color->values[2] *= integrator->spectral_to_reflective[color->color_space][2];

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectiveColorIntegratorComputeReflectorColor(
    _In_ const void *context,
    _In_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    PCREFLECTIVE_INTEGRATOR integrator = (PCREFLECTIVE_INTEGRATOR)context;

    ISTATUS status =
        ColorIntegratorComputeReflectorColor(integrator->color_integrator,
                                             reflector,
                                             color);

    return status;
}

static
void
ReflectiveColorIntegratorFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PREFLECTIVE_INTEGRATOR integrator = (PREFLECTIVE_INTEGRATOR)context;
    ColorIntegratorRelease(integrator->color_integrator);
}

static
void
PopulateForColorSpace(
    _In_ COLOR_SPACE color_space,
    _In_ COLOR3 spectral_color,
    _In_ COLOR3 reflective_color,
    _Out_ float_t values[3]
    )
{
    spectral_color = ColorConvert(spectral_color, color_space);
    reflective_color = ColorConvert(reflective_color, color_space);

    values[0] = reflective_color.values[0] / spectral_color.values[0];
    values[1] = reflective_color.values[1] / spectral_color.values[1];
    values[2] = reflective_color.values[2] / spectral_color.values[2];
}

//
// Static Data
//

static const COLOR_INTEGRATOR_VTABLE reflective_color_integrator_vtable = {
    ReflectiveColorIntegratorComputeSpectrumColor,
    ReflectiveColorIntegratorComputeReflectorColor,
    ReflectiveColorIntegratorFree
};

//
// Functions
//

ISTATUS
ReflectiveColorIntegratorAllocate(
    _In_ PCOLOR_INTEGRATOR base_color_integrator,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    float_t wavelenth = (float_t)1.0;
    float_t intensity = (float_t)1.0;
    PREFLECTOR reflector;
    ISTATUS status = InterpolatedReflectorAllocate(&wavelenth,
                                                   &intensity,
                                                   1,
                                                   &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PSPECTRUM spectrum;
    status = InterpolatedSpectrumAllocate(&wavelenth,
                                          &intensity,
                                          1,
                                          &spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        ReflectorRelease(reflector);
        return status;
    }

    COLOR3 spectral_color;
    status = ColorIntegratorComputeSpectrumColor(base_color_integrator,
                                                 spectrum,
                                                 &spectral_color);

    if (status != ISTATUS_SUCCESS)
    {
        ReflectorRelease(reflector);
        SpectrumRelease(spectrum);
        return status;
    }

    COLOR3 reflector_color;
    status = ColorIntegratorComputeReflectorColor(base_color_integrator,
                                                  reflector,
                                                  &reflector_color);

    ReflectorRelease(reflector);
    SpectrumRelease(spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    REFLECTIVE_INTEGRATOR integrator;
    integrator.color_integrator = base_color_integrator;

    PopulateForColorSpace(COLOR_SPACE_XYZ,
                          spectral_color,
                          reflector_color,
                          integrator.spectral_to_reflective[COLOR_SPACE_XYZ]);
    assert(COLOR_SPACE_XYZ < 2);

    PopulateForColorSpace(COLOR_SPACE_LINEAR_SRGB,
                          spectral_color,
                          reflector_color,
                          integrator.spectral_to_reflective[COLOR_SPACE_LINEAR_SRGB]);
    assert(COLOR_SPACE_LINEAR_SRGB < 2);

    status =
        ColorIntegratorAllocate(&reflective_color_integrator_vtable,
                                &integrator,
                                sizeof(REFLECTIVE_INTEGRATOR),
                                alignof(REFLECTIVE_INTEGRATOR),
                                color_integrator);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ColorIntegratorRetain(base_color_integrator);

    return ISTATUS_SUCCESS;
}