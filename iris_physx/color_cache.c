/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_cache.c

Abstract:

    Interface representing a color matcher.

--*/

#include "iris_physx/color_cache.h"
#include "iris_physx/color_cache_internal.h"
#include "iris_physx/color_integrator_internal.h"

//
// Functions
//

ISTATUS
ColorCacheAddSpectrum(
    _In_ PCOLOR_CACHE color_cache,
    _In_opt_ PCSPECTRUM spectrum
    )
{
    if (color_cache == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    size_t index;
    bool found = SpectrumToColorMapFindIndex(color_cache->spectrum_map,
                                             spectrum,
                                             &index);

    if (found)
    {
        return ISTATUS_SUCCESS;
    }

    COLOR3 color;
    ISTATUS status = ColorIntegratorComputeSpectrumColorStatic(
        color_cache->color_integrator, spectrum, &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumToColorMapAdd(color_cache->spectrum_map,
                                   spectrum,
                                   color);

    return status;
}

ISTATUS
ColorCacheAddReflector(
    _In_ PCOLOR_CACHE color_cache,
    _In_opt_ PCREFLECTOR reflector
    )
{
    if (color_cache == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    size_t index;
    bool found = ReflectorToColorMapFindIndex(color_cache->reflector_map,
                                              reflector,
                                              &index);

    if (found)
    {
        return ISTATUS_SUCCESS;
    }

    COLOR3 color;
    ISTATUS status = ColorIntegratorComputeReflectorColorStatic(
        color_cache->color_integrator, reflector, &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorToColorMapAdd(color_cache->reflector_map,
                                    reflector,
                                    color);

    return status;
}