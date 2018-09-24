/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_internal.h

Abstract:

    The internal routines for a pixel sampler.

--*/

#ifndef _IRIS_PIXEL_SAMPLER_PIXEL_SAMPLER_INTERNAL_
#define _IRIS_PIXEL_SAMPLER_PIXEL_SAMPLER_INTERNAL_

#include "iris_camera/pixel_sampler_vtable.h"

//
// Types
//

struct _PIXEL_SAMPLER {
    PCPIXEL_SAMPLER_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
PixelSamplerGenerateSample(
    _Inout_ struct _PIXEL_SAMPLER *pixel_sampler,
    _Inout_ PRANDOM rng,
    _In_ float_t pixel_sample_min_u,
    _In_ float_t pixel_sample_max_u,
    _In_ float_t pixel_sample_min_v,
    _In_ float_t pixel_sample_max_v,
    _In_ float_t lens_sample_min_u,
    _In_ float_t lens_sample_max_u,
    _In_ float_t lens_sample_min_v,
    _In_ float_t lens_sample_max_v,
    _Out_ size_t *num_samples
    )
{
    assert(pixel_sampler != NULL);
    assert(rng != NULL);
    assert(isfinite(pixel_sample_min_u));
    assert(isfinite(pixel_sample_max_u));
    assert(pixel_sample_min_u < pixel_sample_max_u);
    assert(isfinite(pixel_sample_min_v));
    assert(isfinite(pixel_sample_max_v));
    assert(pixel_sample_min_v < pixel_sample_max_v);
    assert(isfinite(lens_sample_min_u));
    assert(isfinite(lens_sample_max_u));
    assert(lens_sample_min_u < lens_sample_max_u);
    assert(isfinite(lens_sample_min_v));
    assert(isfinite(lens_sample_max_v));
    assert(lens_sample_min_v < lens_sample_max_v);
    assert(num_samples != NULL);

    ISTATUS status =
        pixel_sampler->vtable->prepare_samples_routine(pixel_sampler->data,
                                                       rng,
                                                       pixel_sample_min_u,
                                                       pixel_sample_max_u,
                                                       pixel_sample_min_v,
                                                       pixel_sample_max_v,
                                                       lens_sample_min_u,
                                                       lens_sample_max_u,
                                                       lens_sample_min_v,
                                                       lens_sample_max_v,
                                                       num_samples);

    return status;
}

_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS
PixelSamplerNextSample(
    _Inout_ struct _PIXEL_SAMPLER *pixel_sampler,
    _Inout_ PRANDOM rng,
    _Out_ float_t *pixel_sample_u,
    _Out_ float_t *pixel_sample_v,
    _Out_ float_t *lens_sample_u,
    _Out_ float_t *lens_sample_v
    )
{
    assert(pixel_sampler != NULL);
    assert(rng != NULL);
    assert(pixel_sample_u != NULL);
    assert(pixel_sample_v != NULL);
    assert(lens_sample_u != NULL);
    assert(lens_sample_v != NULL);

    ISTATUS status =
        pixel_sampler->vtable->next_sample_routine(pixel_sampler->data,
                                                   rng,
                                                   pixel_sample_u,
                                                   pixel_sample_v,
                                                   lens_sample_u,
                                                   lens_sample_v);

    return status;
}

#endif // _IRIS_PIXEL_SAMPLER_PIXEL_SAMPLER_INTERNAL_