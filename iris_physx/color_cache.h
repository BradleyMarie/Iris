/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_cache.h

Abstract:

    Caches the xyz color of a spectrum or a reflector.

--*/

#ifndef _IRIS_PHYSX_COLOR_CACHE_
#define _IRIS_PHYSX_COLOR_CACHE_

#include "iris_physx/reflector.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef struct _COLOR_CACHE COLOR_CACHE, *PCOLOR_CACHE;
typedef const COLOR_CACHE *PCCOLOR_CACHE;

//
// Functions
//

ISTATUS
ColorCacheAddSpectrum(
    _In_ PCOLOR_CACHE color_cache,
    _In_opt_ PSPECTRUM spectrum
    );

ISTATUS
ColorCacheAddReflector(
    _In_ PCOLOR_CACHE color_cache,
    _In_opt_ PREFLECTOR reflector
    );

#endif // _IRIS_PHYSX_COLOR_CACHE_