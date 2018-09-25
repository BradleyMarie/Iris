/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_generator_internal.h

Abstract:

    The internal routines for a pixel sampler generator.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_INTERNAL_
#define _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_INTERNAL_

#include "iris_camera/pixel_sampler_generator_vtable.h"

//
// Types
//

struct _PIXEL_SAMPLER_GENERATOR {
    PCPIXEL_SAMPLER_GENERATOR_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
PixelSamplerGeneratorGenerate(
    _In_ const struct _PIXEL_SAMPLER_GENERATOR *pixel_sampler_generator,
    _Out_ PIXEL_SAMPLER *pixel_sampler
    )
{
    assert(pixel_sampler_generator != NULL);
    assert(pixel_sampler != NULL);

    ISTATUS status = pixel_sampler_generator->vtable->generate_routine(
            pixel_sampler_generator->data, pixel_sampler);

    return status;
}

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_INTERNAL_