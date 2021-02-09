/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    image_sampler_internal.h

Abstract:

    The internal routines for a image sampler.

--*/

#ifndef _IRIS_CAMERA_IMAGE_SAMPLER_INTERNAL_
#define _IRIS_CAMERA_IMAGE_SAMPLER_INTERNAL_

#include "iris_camera/image_sampler_vtable.h"

//
// Types
//

struct _IMAGE_SAMPLER {
    PCIMAGE_SAMPLER_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
ISTATUS
ImageSamplerSeed(
    _Inout_ struct _IMAGE_SAMPLER *image_sampler,
    _Inout_ PRANDOM rng
    )
{
    assert(image_sampler != NULL);
    assert(rng != NULL);

    if (image_sampler->vtable->seed_routine == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = image_sampler->vtable->seed_routine(image_sampler->data,
                                                         rng);

    return status;
}

static
inline
ISTATUS
ImageSamplerRandom(
    _Inout_ struct _IMAGE_SAMPLER *image_sampler,
    _Out_ PRANDOM *rng
    )
{
    assert(image_sampler != NULL);
    assert(rng != NULL);

    if (image_sampler->vtable->random_routine == NULL)
    {
        *rng = NULL;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status = image_sampler->vtable->random_routine(image_sampler->data,
                                                           rng);

    return status;
}

static
inline
ISTATUS
ImageSamplerStart(
    _Inout_ struct _IMAGE_SAMPLER *image_sampler,
    _In_ size_t column,
    _In_ size_t num_columns,
    _In_ size_t row,
    _In_ size_t num_rows,
    _Out_ uint32_t *num_samples
    )
{
    assert(image_sampler != NULL);
    assert(num_columns != 0);
    assert(column < num_columns);
    assert(num_rows != 0);
    assert(row < num_rows);
    assert(num_samples != NULL);

    ISTATUS status = image_sampler->vtable->start_routine(image_sampler->data,
                                                          column,
                                                          num_columns,
                                                          row,
                                                          num_rows,
                                                          num_samples);

    return status;
}

static
inline
ISTATUS
ImageSamplerNext(
    _In_ struct _IMAGE_SAMPLER *image_sampler,
    _Inout_ PRANDOM rng,
    _Out_ float_t *pixel_u,
    _Out_ float_t *pixel_v,
    _Out_ float_t *dpixel_u,
    _Out_ float_t *dpixel_v,
    _Out_opt_ float_t *lens_u,
    _Out_opt_ float_t *lens_v
    )
{
    assert(image_sampler != NULL);
    assert(rng != NULL);
    assert(pixel_u != NULL);
    assert(pixel_v != NULL);
    assert(dpixel_u != NULL);
    assert(dpixel_v != NULL);

    ISTATUS status = image_sampler->vtable->next_routine(image_sampler->data,
                                                         rng,
                                                         pixel_u,
                                                         pixel_v,
                                                         dpixel_u,
                                                         dpixel_v,
                                                         lens_u,
                                                         lens_v);

    return status;
}

static
inline
ISTATUS
ImageSamplerDuplicate(
    _In_ const struct _IMAGE_SAMPLER *image_sampler,
    _Out_ struct _IMAGE_SAMPLER **duplicate
    )
{
    assert(image_sampler != NULL);
    assert(duplicate != NULL);

    ISTATUS status =
        image_sampler->vtable->duplicate_routine(image_sampler->data,
                                                 duplicate);

    return status;
}

#endif // _IRIS_CAMERA_IMAGE_SAMPLER_INTERNAL_