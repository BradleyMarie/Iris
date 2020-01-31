/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    xyz_spectra.c

Abstract:

    Implements the XYZ spectrum, reflector, and color integrator.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/xyz_spectra.h"

//
// Constants
//

#define X_WAVELENGTH ((float_t)1.0)
#define Y_WAVELENGTH ((float_t)2.0)
#define Z_WAVELENGTH ((float_t)3.0)

//
// Types
//

typedef struct _XYZ_SPECTRUM {
    float_t x;
    float_t y;
    float_t z;
} XYZ_SPECTRUM, *PXYZ_SPECTRUM;

typedef const XYZ_SPECTRUM *PCXYZ_SPECTRUM;

typedef struct _XYZ_REFLECTOR {
    float_t x;
    float_t y;
    float_t z;
} XYZ_REFLECTOR, *PXYZ_REFLECTOR;

typedef const XYZ_REFLECTOR *PCXYZ_REFLECTOR;

//
// Static Functions
//

static
ISTATUS
XyzSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCXYZ_SPECTRUM xyz_spectrum = (PCXYZ_SPECTRUM)context;

    if (wavelength == X_WAVELENGTH)
    {
        *intensity = xyz_spectrum->x;
    }
    else if (wavelength == Y_WAVELENGTH)
    {
        *intensity = xyz_spectrum->y;
    }
    else if (wavelength == Z_WAVELENGTH)
    {
        *intensity = xyz_spectrum->z;
    }
    else
    {
        *intensity = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
XyzReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCXYZ_REFLECTOR xyz_reflector = (PCXYZ_REFLECTOR)context;

    if (wavelength == X_WAVELENGTH)
    {
        *reflectance = xyz_reflector->x;
    }
    else if (wavelength == Y_WAVELENGTH)
    {
        *reflectance = xyz_reflector->y;
    }
    else if (wavelength == Z_WAVELENGTH)
    {
        *reflectance = xyz_reflector->z;
    }
    else
    {
        *reflectance = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
XyzReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCXYZ_REFLECTOR xyz_reflector = (PCXYZ_REFLECTOR)context;

    *albedo = xyz_reflector->y;

    return ISTATUS_SUCCESS;
}

ISTATUS
XyzColorIntegratorComputeSpectrumColor(
    _In_ const void *context,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    ISTATUS status = SpectrumSample(spectrum,
                                    X_WAVELENGTH,
                                    &color->x);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumSample(spectrum,
                            Y_WAVELENGTH,
                            &color->y);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumSample(spectrum,
                            Z_WAVELENGTH,
                            &color->z);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
XyzColorIntegratorComputeReflectorColor(
    _In_ const void *context,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    ISTATUS status = ReflectorReflect(reflector,
                                      X_WAVELENGTH,
                                      &color->x);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflect(reflector,
                              Y_WAVELENGTH,
                              &color->y);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflect(reflector,
                              Z_WAVELENGTH,
                              &color->z);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const SPECTRUM_VTABLE xyz_spectrum_vtable = {
    XyzSpectrumSample,
    NULL
};

static const REFLECTOR_VTABLE xyz_reflector_vtable = {
    XyzReflectorReflect,
    XyzReflectorGetAlbedo,
    NULL
};

static const COLOR_INTEGRATOR_VTABLE xyz_integrator_vtable = {
    XyzColorIntegratorComputeSpectrumColor,
    XyzColorIntegratorComputeReflectorColor,
    NULL
};

//
// Functions
//

ISTATUS
XyzSpectrumAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (!isfinite(x) || x < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(y) || y < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(z) || z < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    XYZ_SPECTRUM xyz_spectrum;
    xyz_spectrum.x = x;
    xyz_spectrum.y = y;
    xyz_spectrum.z = z;

    ISTATUS status = SpectrumAllocate(&xyz_spectrum_vtable,
                                      &xyz_spectrum,
                                      sizeof(XYZ_SPECTRUM),
                                      alignof(XYZ_SPECTRUM),
                                      spectrum);

    return status;
}

ISTATUS
XyzReflectorAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PREFLECTOR *reflector
    )
{
    if (!isfinite(x) || x < (float_t)0.0 || (float_t)1.0 < x)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(y) || y < (float_t)0.0 || (float_t)1.0 < y)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(z) || z < (float_t)0.0 || (float_t)1.0 < z)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    XYZ_REFLECTOR xyz_reflector;
    xyz_reflector.x = x;
    xyz_reflector.y = y;
    xyz_reflector.z = z;

    ISTATUS status = ReflectorAllocate(&xyz_reflector_vtable,
                                       &xyz_reflector,
                                       sizeof(XYZ_REFLECTOR),
                                       alignof(XYZ_REFLECTOR),
                                       reflector);

    return status;
}

ISTATUS
XyzColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status = ColorIntegratorAllocate(&xyz_integrator_vtable,
                                             NULL,
                                             0,
                                             0,
                                             color_integrator);

    return status;
}