/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sampler_vtable.h

Abstract:

    Function pointer types for light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_
#define _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_

#include "iris_physx/light_sample_collector.h"

//
// Types
//

typedef
ISTATUS
(*PLIGHT_SAMPLER_SAMPLE_LIGHTS_ROUTINE)(
    _In_opt_ const void* context,
    _In_ POINT3 point,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_