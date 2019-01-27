/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    spectra.c

Abstract:

    Implements the test spectrum, reflector, and color integrator.

--*/

#include <stdalign.h>

#include "test_util/spectra.h"

//
// Constants
//

#define X_WAVELENGTH ((float_t)1.0)
#define Y_WAVELENGTH ((float_t)2.0)
#define Z_WAVELENGTH ((float_t)3.0)

//
// Types
//

typedef struct _TEST_SPECTRUM {
    float_t x;
    float_t y;
    float_t z;
} TEST_SPECTRUM, *PTEST_SPECTRUM;

typedef const TEST_SPECTRUM *PCTEST_SPECTRUM;

typedef struct _TEST_REFLECTOR {
    float_t x;
    float_t y;
    float_t z;
} TEST_REFLECTOR, *PTEST_REFLECTOR;

typedef const TEST_REFLECTOR *PCTEST_REFLECTOR;

//
// Static Functions
//

static
ISTATUS
TestSpectrumSample(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    PCTEST_SPECTRUM test_spectrum = (PCTEST_SPECTRUM)context;

    if (wavelength == X_WAVELENGTH)
    {
        *intensity = test_spectrum->x;
    }
    else if (wavelength == Y_WAVELENGTH)
    {
        *intensity = test_spectrum->y;
    }
    else if (wavelength == Z_WAVELENGTH)
    {
        *intensity = test_spectrum->z;
    }
    else
    {
        *intensity = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TestReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCTEST_REFLECTOR test_reflector = (PCTEST_REFLECTOR)context;

    if (wavelength == X_WAVELENGTH)
    {
        *reflectance = test_reflector->x;
    }
    else if (wavelength == Y_WAVELENGTH)
    {
        *reflectance = test_reflector->y;
    }
    else if (wavelength == Z_WAVELENGTH)
    {
        *reflectance = test_reflector->z;
    }
    else
    {
        *reflectance = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TestReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCTEST_REFLECTOR test_reflector = (PCTEST_REFLECTOR)context;

    float_t sum = test_reflector->x + test_reflector->y + test_reflector->z;
    *albedo = sum / (float_t)3.0;

    return ISTATUS_SUCCESS;
}

ISTATUS
TestColorIntegratorComputeSpectrumColor(
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
TestColorIntegratorComputeReflectorColor(
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

static const SPECTRUM_VTABLE test_spectrum_vtable = {
    TestSpectrumSample,
    NULL
};

static const REFLECTOR_VTABLE test_reflector_vtable = {
    TestReflectorReflect,
    TestReflectorGetAlbedo,
    NULL
};

//
// Functions
//

ISTATUS
TestSpectrumAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (isinf(x) || isless(x, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (isinf(y) || isless(y, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (isinf(z) || isless(z, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    TEST_SPECTRUM test_spectrum;
    test_spectrum.x = x;
    test_spectrum.y = y;
    test_spectrum.z = z;

    ISTATUS status = SpectrumAllocate(&test_spectrum_vtable,
                                      &test_spectrum,
                                      sizeof(TEST_SPECTRUM),
                                      alignof(TEST_SPECTRUM),
                                      spectrum);

    return status;
}

ISTATUS
TestReflectorAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PREFLECTOR *reflector
    )
{
    if (isinf(x) || isless(x, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (isinf(y) || isless(y, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (isinf(z) || isless(z, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    TEST_REFLECTOR test_reflector;
    test_reflector.x = x;
    test_reflector.y = y;
    test_reflector.z = z;

    ISTATUS status = ReflectorAllocate(&test_reflector_vtable,
                                       &test_reflector,
                                       sizeof(TEST_REFLECTOR),
                                       alignof(TEST_REFLECTOR),
                                       reflector);

    return status;
}

ISTATUS
TestColorIntegratorAllocate(
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    ISTATUS status =
        ColorIntegratorAllocate(TestColorIntegratorComputeSpectrumColor,
                                TestColorIntegratorComputeReflectorColor,
                                color_integrator);

    return status;
}