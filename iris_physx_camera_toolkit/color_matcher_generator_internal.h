/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_generator_internal.h

Abstract:

    The internal routines for an color matcher generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_INTERNAL_
#define _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_INTERNAL_

#include "iris_physx_camera_toolkit/color_matcher_generator_vtable.h"

//
// Types
//

struct _COLOR_MATCHER_GENERATOR {
    PCCOLOR_MATCHER_GENERATOR_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
ColorMatcherGeneratorGenerate(
    _In_ const struct _COLOR_MATCHER_GENERATOR *color_matcher_generator,
    _Out_ PCOLOR_MATCHER *color_matcher
    )
{
    assert(color_matcher_generator != NULL);
    assert(color_matcher != NULL);

    PCOLOR_MATCHER result;
    ISTATUS status = color_matcher_generator->vtable->generate_routine(
        color_matcher_generator->data, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *color_matcher = result;

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_PHYSX_CAMERA_COLOR_MATCHER_GENERATOR_INTERNAL_