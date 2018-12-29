/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pixel_sampler_generator_vtable.h

Abstract:

    The vtable for a pixel sampler generator.

--*/

#ifndef _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_VTABLE_
#define _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_VTABLE_

#include "iris_camera/pixel_sampler.h"

typedef
ISTATUS
(*PPIXEL_SAMPLER_GENERATOR_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PPIXEL_SAMPLER *pixel_sampler
    );

typedef struct _PIXEL_SAMPLER_GENERATOR_VTABLE {
    PPIXEL_SAMPLER_GENERATOR_GENERATE_ROUTINE generate_routine;
    PFREE_ROUTINE free_routine;
} PIXEL_SAMPLER_GENERATOR_VTABLE, *PPIXEL_SAMPLER_GENERATOR_VTABLE;

typedef const PIXEL_SAMPLER_GENERATOR_VTABLE *PCPIXEL_SAMPLER_GENERATOR_VTABLE;

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_GENERATOR_VTABLE_