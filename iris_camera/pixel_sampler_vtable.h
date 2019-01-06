/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_vtable.h

Abstract:

    The vtable for a pixel sampler.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_VTABLE_
#define _IRIS_CAMERA_PIXEL_SAMPLER_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef const PIXEL_SAMPLER *PCPIXEL_SAMPLER;

typedef
ISTATUS
(*PPIXEL_SAMPLER_PREPARE_SAMPLES_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v
    );

typedef
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
(*PPIXEL_SAMPLER_NEXT_SAMPLE_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    );

typedef
ISTATUS
(*PPIXEL_SAMPLER_DUPLICATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PPIXEL_SAMPLER *duplicate
    );

typedef struct _PIXEL_SAMPLER_VTABLE {
    PPIXEL_SAMPLER_PREPARE_SAMPLES_ROUTINE prepare_samples_routine;
    PPIXEL_SAMPLER_NEXT_SAMPLE_ROUTINE next_sample_routine;
    PPIXEL_SAMPLER_DUPLICATE_ROUTINE duplicate_routine;
    PFREE_ROUTINE free_routine;
} PIXEL_SAMPLER_VTABLE, *PPIXEL_SAMPLER_VTABLE;

typedef const PIXEL_SAMPLER_VTABLE *PCPIXEL_SAMPLER_VTABLE;

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_VTABLE_