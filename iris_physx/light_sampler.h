/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler.h

Abstract:

    Samples the lights in a scene from a given point to control which lights
    are used for shading.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_
#define _IRIS_PHYSX_LIGHT_SAMPLER_

#include "iris_physx/light_sampler_vtable.h"

//
// Types
//

typedef struct _LIGHT_SAMPLER LIGHT_SAMPLER, *PLIGHT_SAMPLER;
typedef const LIGHT_SAMPLER *PCLIGHT_SAMPLER;

//
// Functions
//

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
LightSamplerPrepareSamples(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point
    );

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
LightSamplerNextSample(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_