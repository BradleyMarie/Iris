/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    directional_light.c

Abstract:

    Implements a directional light.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/directional_light.h"

//
// Types
//

typedef struct _DIRECTIONAL_LIGHT {
    VECTOR3 to_light;
    PSPECTRUM spectrum;
} DIRECTIONAL_LIGHT, *PDIRECTIONAL_LIGHT;

typedef const DIRECTIONAL_LIGHT *PCDIRECTIONAL_LIGHT;

//
// Static Functions
//

static
ISTATUS
DirectionalLightSample(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCDIRECTIONAL_LIGHT directional_light = (PCDIRECTIONAL_LIGHT)context;

    RAY ray_to_light = RayCreate(hit_point, directional_light->to_light);

    bool visible;
    ISTATUS status = VisibilityTesterTestAnyDistance(visibility_tester,
                                                     ray_to_light,
                                                     &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
    }
    else
    {
        *spectrum = directional_light->spectrum;
    }

    *to_light = directional_light->to_light;
    *pdf = (float_t)INFINITY;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
DirectionalLightComputeEmissive(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    *spectrum = NULL;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
DirectionalLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    *spectrum = NULL;
    *pdf = (float_t)0.0;

    return ISTATUS_SUCCESS;
}

static
void
DirectionalLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PDIRECTIONAL_LIGHT directional_light = (PDIRECTIONAL_LIGHT)context;

    SpectrumRelease(directional_light->spectrum);
}

//
// Static Variables
//

static const LIGHT_VTABLE directional_light_vtable = {
    DirectionalLightSample,
    DirectionalLightComputeEmissive,
    DirectionalLightComputeEmissiveWithPdf,
    DirectionalLightFree
};

//
// Functions
//

ISTATUS
DirectionalLightAllocate(
    _In_ VECTOR3 to_light,
    _In_ PSPECTRUM spectrum,
    _Out_ PLIGHT *light
    )
{
    if (!VectorValidate(to_light))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    DIRECTIONAL_LIGHT directional_light;
    directional_light.to_light = VectorNormalize(to_light, NULL, NULL);
    directional_light.spectrum = spectrum;

    ISTATUS status = LightAllocate(&directional_light_vtable,
                                   &directional_light,
                                   sizeof(DIRECTIONAL_LIGHT),
                                   alignof(DIRECTIONAL_LIGHT),
                                   light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}