/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    infinite_environmental_light.c

Abstract:

    Implements a infinite environmental light.

--*/

#include <stdalign.h>

#include "iris_advanced_toolkit/sample_geometry.h"
#include "iris_physx_toolkit/infinite_environmental_light.h"

//
// Types
//

typedef struct _INFINITE_LIGHT {
    PSPECTRUM_TEXTURE texture;
} INFINITE_LIGHT, *PINFINITE_LIGHT;

typedef const INFINITE_LIGHT *PCINFINITE_LIGHT;

//
// Static Functions
//

static
ISTATUS
InfiniteEnvironmentalLightSample(
    _In_ const void *context,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    ISTATUS status = SampleHemisphereUniformly(surface_normal,
                                               rng,
                                               to_light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumTextureSample(infinite_light->texture,
                                   *to_light,
                                   NULL,
                                   compositor,
                                   spectrum);

    *pdf = iris_inv_two_pi;

    return status;
}

static
ISTATUS
InfiniteEnvironmentalLightComputeEmissive(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    ISTATUS status = SpectrumTextureSample(infinite_light->texture,
                                           to_light,
                                           NULL,
                                           compositor,
                                           spectrum);

    return status;
}

static
ISTATUS
InfiniteEnvironmentalLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    ISTATUS status = SpectrumTextureSample(infinite_light->texture,
                                           to_light,
                                           NULL,
                                           compositor,
                                           spectrum);

    *pdf = iris_inv_two_pi;

    return status;
}

static
void
InfiniteEnvironmentalLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PINFINITE_LIGHT infinite_light = (PINFINITE_LIGHT)context;

    SpectrumTextureRelease(infinite_light->texture);
}

//
// Static Variables
//

static const ENVIRONMENTAL_LIGHT_VTABLE infinite_light_vtable = {
    InfiniteEnvironmentalLightSample,
    InfiniteEnvironmentalLightComputeEmissive,
    InfiniteEnvironmentalLightComputeEmissiveWithPdf,
    InfiniteEnvironmentalLightFree
};

//
// Functions
//

ISTATUS
InfiniteEnvironmentalLightAllocate(
    _In_ PSPECTRUM_TEXTURE texture,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (environmental_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    INFINITE_LIGHT infinite_light;
    infinite_light.texture = texture;

    ISTATUS status = EnvironmentalLightAllocate(&infinite_light_vtable,
                                                &infinite_light,
                                                sizeof(INFINITE_LIGHT),
                                                alignof(INFINITE_LIGHT),
                                                environmental_light,
                                                light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SpectrumTextureRetain(texture);

    return ISTATUS_SUCCESS;
}