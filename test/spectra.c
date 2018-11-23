/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectra.c

Abstract:

    Implements the test spectrum, reflector, and tone mapper.

--*/

#include <stdalign.h>

#include "test/spectra.h"

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

typedef struct _TEST_TONE_MAPPER {
    float_t x;
    float_t y;
    float_t z;
} TEST_TONE_MAPPER, *PTEST_TONE_MAPPER;

typedef const TEST_TONE_MAPPER *PCTEST_TONE_MAPPER;

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
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    PCTEST_REFLECTOR test_reflector = (PCTEST_REFLECTOR)context;

    if (wavelength == X_WAVELENGTH)
    {
        *outgoing_intensity = incoming_intensity * test_reflector->x;
    }
    else if (wavelength == Y_WAVELENGTH)
    {
        *outgoing_intensity = incoming_intensity * test_reflector->y;
    }
    else if (wavelength == Z_WAVELENGTH)
    {
        *outgoing_intensity = incoming_intensity * test_reflector->z;
    }
    else
    {
        *outgoing_intensity = (float_t)0.0;
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
TestToneMapperAddSample(
    _In_ void *context,
    _In_opt_ PCSPECTRUM spectrum
    )
{
    PTEST_TONE_MAPPER test_tone_mapper = (PTEST_TONE_MAPPER)context;

    float_t intensity;
    ISTATUS status = SpectrumSample(spectrum,
                                    X_WAVELENGTH,
                                    &intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    test_tone_mapper->x += intensity;

    status = SpectrumSample(spectrum,
                            Y_WAVELENGTH,
                            &intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    test_tone_mapper->y += intensity;

    status = SpectrumSample(spectrum,
                            Z_WAVELENGTH,
                            &intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    test_tone_mapper->z += intensity;

    return ISTATUS_SUCCESS;
}

ISTATUS
TestToneMapperComputeTone(
    _In_ const void *context,
    _Out_ PCOLOR3 color
    )
{
    PCTEST_TONE_MAPPER test_tone_mapper = (PCTEST_TONE_MAPPER)context;

    *color = ColorCreate(test_tone_mapper->x,
                         test_tone_mapper->y,
                         test_tone_mapper->z);

    return ISTATUS_SUCCESS;
}

ISTATUS
TestToneMapperClear(
    _Inout_ void *context
    )
{
    PTEST_TONE_MAPPER test_tone_mapper = (PTEST_TONE_MAPPER)context;

    test_tone_mapper->x = (float_t)0.0;
    test_tone_mapper->y = (float_t)0.0;
    test_tone_mapper->z = (float_t)0.0;

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

static const TONE_MAPPER_VTABLE test_tone_mapper_vtable = {
    TestToneMapperAddSample,
    TestToneMapperComputeTone,
    TestToneMapperClear,
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
TestToneMapperAllocate(
    _Out_ PTONE_MAPPER *tone_mapper
    )
{
    if (tone_mapper == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    TEST_TONE_MAPPER test_tone_mapper;
    test_tone_mapper.x = (float_t)0.0;
    test_tone_mapper.y = (float_t)0.0;
    test_tone_mapper.z = (float_t)0.0;

    ISTATUS status = ToneMapperAllocate(&test_tone_mapper_vtable,
                                        &test_tone_mapper,
                                        sizeof(TEST_TONE_MAPPER),
                                        alignof(TEST_TONE_MAPPER),
                                        tone_mapper);

    return status;
}