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
(*PIMAGE_SAMPLER_PREPARE_IMAGE_SAMPLES_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t num_columns,
    _In_ size_t num_rows
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_PREPARE_PIXEL_SAMPLES_ROUTINE)(
    _In_ void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t column,
    _In_ size_t row,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _Out_ size_t *num_samples
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_GET_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v,
    _Out_ float_t *dpixel_sample_u,
    _Out_ float_t *dpixel_sample_v
    );

typedef
ISTATUS
(*PIMAGE_SAMPLER_DUPLICATE_ROUTINE)(
    _In_opt_ const void *context,
    _Out_ PIMAGE_SAMPLER *duplicate
    );

typedef struct _IMAGE_SAMPLER_VTABLE {
    PIMAGE_SAMPLER_PREPARE_IMAGE_SAMPLES_ROUTINE prepare_image_samples_routine;
    PIMAGE_SAMPLER_PREPARE_PIXEL_SAMPLES_ROUTINE prepare_pixel_samples_routine;
    PIMAGE_SAMPLER_GET_SAMPLE_ROUTINE get_sample_routine;
    PIMAGE_SAMPLER_DUPLICATE_ROUTINE duplicate_routine;
    PFREE_ROUTINE free_routine;
} IMAGE_SAMPLER_VTABLE, *PIMAGE_SAMPLER_VTABLE;

typedef const IMAGE_SAMPLER_VTABLE *PCIMAGE_SAMPLER_VTABLE;

#endif // _IRIS_CAMERA_IMAGE_SAMPLER_VTABLE_