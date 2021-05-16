/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    light_sampler.h

Abstract:

    Samples the lights in a scene from a given point to control which lights
    are used for shading.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_
#define _IRIS_PHYSX_LIGHT_SAMPLER_

#include "iris_physx/light_sample_list.h"
#include "iris_physx/light_sampler_vtable.h"

//
// Types
//

typedef struct _LIGHT_SAMPLER LIGHT_SAMPLER, *PLIGHT_SAMPLER;
typedef const LIGHT_SAMPLER *PCLIGHT_SAMPLER;

//
// Functions
//

ISTATUS
LightSamplerAllocate(
    _In_ PCLIGHT_SAMPLER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PLIGHT_SAMPLER *light_sampler
    );

ISTATUS
LightSamplerSample(
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ POINT3 point,
    _Inout_ PRANDOM rng,
    _Inout_ PLIGHT_SAMPLE_LIST light_sample_list
    );

void
LightSamplerRetain(
    _In_opt_ PLIGHT_SAMPLER light_sampler
    );

void
LightSamplerRelease(
    _In_opt_ _Post_invalid_ PLIGHT_SAMPLER light_sampler
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_