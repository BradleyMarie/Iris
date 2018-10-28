/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sampler_vtable.h

Abstract:

    Function pointer types for light sampler.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_
#define _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_

#include "iris_physx/light.h"

//
// Types
//

typedef
ISTATUS
(*PLIGHT_SAMPLER_PREPARE_SAMPLES_ROUTINE)(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _In_ POINT3 point
    );

typedef
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
(*PLIGHT_SAMPLER_NEXT_SAMPLE_ROUTINE)(
    _Inout_opt_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );
    
#endif // _IRIS_PHYSX_LIGHT_SAMPLER_VTABLE_