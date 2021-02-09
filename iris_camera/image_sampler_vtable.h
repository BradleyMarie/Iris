/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_sampler_vtable.h

Abstract:

    The vtable for a image sampler.

--*/

#ifndef _IRIS_CAMERA_IMAGE_SAMPLER_VTABLE_
#define _IRIS_CAMERA_IMAGE_SAMPLER_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _IMAGE_SAMPLER IMAGE_SAMPLER, *PIMAGE_SAMPLER;
typedef const IMAGE_SAMPLER *PCIMAGE_SAMPLER;

typedef
ISTATUS
(*PIMAGE_SAMPLER_SEED_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_RANDOM_ROUTINE)(
    _In_ void *context,
    _Out_ PRANDOM *rng
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_START_ROUTINE)(
    _In_ void *context,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _Out_ uint32_t *num_samples
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_NEXT_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _Out_ float_t *pixel_u,
    _Out_ float_t *pixel_v,
    _Out_ float_t *dpixel_u,
    _Out_ float_t *dpixel_v,
    _Out_opt_ float_t *lens_u,
    _Out_opt_ float_t *lens_v
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_DUPLICATE_ROUTINE)(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    );

typedef struct _IMAGE_SAMPLER_VTABLE {
    PIMAGE_SAMPLER_SEED_ROUTINE seed_routine;
    PIMAGE_SAMPLER_RANDOM_ROUTINE random_routine;
    PIMAGE_SAMPLER_START_ROUTINE start_routine;
    PIMAGE_SAMPLER_NEXT_ROUTINE next_routine;
    PIMAGE_SAMPLER_DUPLICATE_ROUTINE duplicate_routine;
    PFREE_ROUTINE free_routine;
} IMAGE_SAMPLER_VTABLE, *PIMAGE_SAMPLER_VTABLE;

typedef const IMAGE_SAMPLER_VTABLE *PCIMAGE_SAMPLER_VTABLE;

#endif // _IRIS_CAMERA_IMAGE_SAMPLER_VTABLE_