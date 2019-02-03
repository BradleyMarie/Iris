/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sampler.h

Abstract:

    Samples the lights in a scene from a given point to control which lights
    are used for shading.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_
#define _IRIS_PHYSX_LIGHT_SAMPLER_

#include "iris_physx/light.h"

//
// Types
//

typedef struct _LIGHT_SAMPLER LIGHT_SAMPLER, *PLIGHT_SAMPLER;
typedef const LIGHT_SAMPLER *PCLIGHT_SAMPLER;

//
// Functions
//

ISTATUS
LightSamplerCollectSamples(
    _Inout_ PLIGHT_SAMPLER light_sampler,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point,
    _Out_ size_t *num_samples
    );

ISTATUS
LightSamplerGetSample(
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ size_t sample_index,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_