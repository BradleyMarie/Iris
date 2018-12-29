/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_generator.h

Abstract:

    Generates an color matcher generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_
#define _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_

#include "iris_physx_camera_toolkit/color_matcher_generator_vtable.h"

//
// Types
//

typedef struct _COLOR_MATCHER_GENERATOR COLOR_MATCHER_GENERATOR;
typedef COLOR_MATCHER_GENERATOR *PCOLOR_MATCHER_GENERATOR;
typedef const COLOR_MATCHER_GENERATOR *PCCOLOR_MATCHER_GENERATOR;

//
// Functions
//

ISTATUS
ColorMatcherGeneratorAllocate(
    _In_ PCCOLOR_MATCHER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_MATCHER_GENERATOR *color_matcher_generator
    );

void
ColorMatcherGeneratorFree(
    _In_opt_ _Post_invalid_ PCOLOR_MATCHER_GENERATOR color_matcher_generator
    );

#endif // _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_