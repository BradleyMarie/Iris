/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_generator_vtable.h

Abstract:

    The vtable for an color matcher generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_VTABLE_
#define _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_VTABLE_

#include "iris_physx/iris_physx.h"
#include "iris_physx_camera_toolkit/color_matcher_generator.h"

typedef
ISTATUS
(*PCOLOR_MATCHER_GENERATOR_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PCOLOR_MATCHER *color_matcher
    );

typedef struct _COLOR_MATCHER_GENERATOR_VTABLE {
    PCOLOR_MATCHER_GENERATOR_GENERATE_ROUTINE generate_routine;
    PFREE_ROUTINE free_routine;
} COLOR_MATCHER_GENERATOR_VTABLE, *PCOLOR_MATCHER_GENERATOR_VTABLE;

typedef const COLOR_MATCHER_GENERATOR_VTABLE *PCCOLOR_MATCHER_GENERATOR_VTABLE;

#endif // _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_VTABLE_