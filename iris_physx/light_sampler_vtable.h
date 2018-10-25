/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler_vtable.h

Abstract:

    The vtable for a light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_
#define _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/light.h"

//
// Types
//

typedef
ISTATUS
(*PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ POINT3 point
    );

typedef
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
(*PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE)(
    _Inout_opt_ void *context,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

typedef struct _LIGHT_SAMPLER_VTABLE {
    PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine;
    PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine;
    PFREE_ROUTINE free_routine;
} LIGHT_SAMPLER_VTABLE, *PLIGHT_SAMPLER_VTABLE;

typedef const LIGHT_SAMPLER_VTABLE *PCLIGHT_SAMPLER_VTABLE;

#endif // _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_