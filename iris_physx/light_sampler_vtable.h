/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    light_sampler_vtable.h

Abstract:

    Function pointer types for light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_
#define _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_

#include "iris_physx/color_cache.h"
#include "iris_physx/light_sample_collector.h"

//
// Types
//

typedef
ISTATUS
(*PLIGHT_SAMPLER_SAMPLE_ROUTINE)(
    _In_opt_ const void* context,
    _In_ POINT3 point,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector
    );

typedef struct _LIGHT_SAMPLER_VTABLE {
    PLIGHT_SAMPLER_SAMPLE_ROUTINE sample_routine;
    PCACHE_COLORS_ROUTINE cache_colors_routine;
    PFREE_ROUTINE free_routine;
} LIGHT_SAMPLER_VTABLE, *PLIGHT_SAMPLER_VTABLE;

typedef const LIGHT_SAMPLER_VTABLE *PCLIGHT_SAMPLER_VTABLE;

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_