/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    point_light.c

Abstract:

    Implements a point light.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/point_light.h"

//
// Types
//

typedef struct _POINT_LIGHT {
    POINT3 location;
    PSPECTRUM spectrum;
} POINT_LIGHT, *PPOINT_LIGHT;

typedef const POINT_LIGHT *PCPOINT_LIGHT;

//
// Static Functions
//

static
inline
ISTATUS
PointLightCheckVisibility(
    _In_ PCPOINT_LIGHT point_light,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Out_ PVECTOR3 direction_to_light,
    _Out_ float_t *distance_to_light_squared,
    _Out_ bool *visible
    )
{
    *direction_to_light = PointSubtract(point_light->location, hit_point);

    float_t distance_to_light;
    *direction_to_light = VectorNormalize(*direction_to_light,
                                          distance_to_light_squared,
                                          &distance_to_light);

    float_t dp = VectorDotProduct(*direction_to_light, surface_normal);

    if (dp <= (float_t)0.0)
    {
        *visible = false;
        return ISTATUS_SUCCESS;
    }

    RAY ray_to_light = RayCreate(hit_point, *direction_to_light);

    ISTATUS status = VisibilityTesterTest(visibility_tester,
                                          ray_to_light,
                                          distance_to_light,
                                          visible);

    return status;
}

static
ISTATUS
PointLightSample(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCPOINT_LIGHT point_light = (PCPOINT_LIGHT)context;

    bool visible;
    float_t distance_to_light_squared;
    ISTATUS status = PointLightCheckVisibility(point_light,
                                               hit_point,
                                               surface_normal,
                                               visibility_tester,
                                               to_light,
                                               &distance_to_light_squared,
                                               &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t attenuation = (float_t)1.0 / distance_to_light_squared;

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 point_light->spectrum,
                                                 attenuation,
                                                 spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = (float_t)INFINITY;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PointLightComputeEmissive(
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
PointLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    *pdf = (float_t)0.0;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
PointLightCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PCPOINT_LIGHT point_light = (PCPOINT_LIGHT)context;

    ISTATUS status = ColorCacheAddSpectrum(color_cache, point_light->spectrum);

    return status;
}

static
void
PointLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PPOINT_LIGHT point_light = (PPOINT_LIGHT)context;

    SpectrumRelease(point_light->spectrum);
}

//
// Static Variables
//

static const LIGHT_VTABLE point_light_vtable = {
    PointLightSample,
    PointLightComputeEmissive,
    PointLightComputeEmissiveWithPdf,
    PointLightCacheColors,
    PointLightFree
};

//
// Functions
//

ISTATUS
PointLightAllocate(
    _In_ POINT3 location,
    _In_ PSPECTRUM spectrum,
    _Out_ PLIGHT *light
    )
{
    if (!PointValidate(location))
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

    POINT_LIGHT point_light;
    point_light.location = location;
    point_light.spectrum = spectrum;

    ISTATUS status = LightAllocate(&point_light_vtable,
                                   &point_light,
                                   sizeof(POINT_LIGHT),
                                   alignof(POINT_LIGHT),
                                   light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}