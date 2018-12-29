/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_internal.h

Abstract:

    The internal routines for a color matcher.

--*/

#ifndef _IRIS_PHYSX_COLOR_MATCHER_INTERNAL_
#define _IRIS_PHYSX_COLOR_MATCHER_INTERNAL_

#include "iris_physx/color_matcher_vtable.h"

//
// Types
//

struct _COLOR_MATCHER {
    PCCOLOR_MATCHER_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
void
ColorMatcherInitialize(
    _In_ PCCOLOR_MATCHER_VTABLE vtable,
    _In_opt_ void *data,
    _Out_ struct _COLOR_MATCHER *color_matcher
    )
{
    assert(vtable != NULL);
    assert(color_matcher != NULL);
    
    color_matcher->vtable = vtable;
    color_matcher->data = data;
}

static
inline
ISTATUS
ColorMatcherAddSample(
    _In_ struct _COLOR_MATCHER *color_matcher,
    _In_opt_ PCSPECTRUM spectrum
    )
{
    assert(color_matcher != NULL);

    ISTATUS status =
        color_matcher->vtable->add_sample_routine(color_matcher->data,
                                                  spectrum);

    return status;
}

#endif // _IRIS_PHYSX_COLOR_MATCHER_INTERNAL_