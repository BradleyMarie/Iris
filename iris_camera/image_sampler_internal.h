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
ImageSamplerPreparePixelSamples(
    _Inout_ struct _IMAGE_SAMPLER *image_sampler,
    _Inout_ PRANDOM rng,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _Out_ size_t *num_samples
    )
{
    assert(image_sampler != NULL);
    assert(rng != NULL);
    assert(isfinite(pixel_min_u));
    assert(isfinite(pixel_max_u));
    assert(pixel_min_u <= pixel_max_u);
    assert(isfinite(pixel_min_v));
    assert(isfinite(pixel_max_v));
    assert(pixel_min_v <= pixel_max_v);
    assert(isfinite(lens_min_u));
    assert(isfinite(lens_max_u));
    assert(lens_min_u <= lens_max_u);
    assert(isfinite(lens_min_v));
    assert(isfinite(lens_max_v));
    assert(lens_min_v <= lens_max_v);
    assert(num_samples != NULL);

    ISTATUS status =
        image_sampler->vtable->prepare_pixel_samples_routine(image_sampler->data,
                                                             rng,
                                                             pixel_min_u,
                                                             pixel_max_u,
                                                             pixel_min_v,
                                                             pixel_max_v,
                                                             lens_min_u,
                                                             lens_max_u,
                                                             lens_min_v,
                                                             lens_max_v,
                                                             num_samples);

    return status;
}

static
inline
ISTATUS
ImageSamplerGetSample(
    _In_ const struct _IMAGE_SAMPLER *image_sampler,
    _Inout_ PRANDOM rng,
    _In_ size_t sample_index,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    )
{
    assert(image_sampler != NULL);
    assert(rng != NULL);
    assert(pixel_sample_u != NULL);
    assert(pixel_sample_v != NULL);
    assert(lens_sample_u != NULL);
    assert(lens_sample_v != NULL);

    ISTATUS status =
        image_sampler->vtable->get_sample_routine(image_sampler->data,
                                                  rng,
                                                  sample_index,
                                                  pixel_sample_u,
                                                  pixel_sample_v,
                                                  lens_sample_u,
                                                  lens_sample_v);

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