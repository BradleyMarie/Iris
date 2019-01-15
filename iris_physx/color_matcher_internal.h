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

#include <stdatomic.h>

//
// Types
//

struct _COLOR_MATCHER {
    PCCOLOR_MATCHER_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
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
    color_matcher->reference_count = 1;
}

static
inline
ISTATUS
ColorMatcherCompute(
    _In_ const struct _COLOR_MATCHER *color_matcher,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    assert(color_matcher != NULL);
    assert(color != NULL);

    ISTATUS status =
        color_matcher->vtable->compute_routine(color_matcher->data,
                                               spectrum,
                                               color);

    return status;
}

#endif // _IRIS_PHYSX_COLOR_MATCHER_INTERNAL_