/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher.h

Abstract:

    Interface representing a color matcher.

--*/

#ifndef _IRIS_PHYSX_COLOR_MATCHER_
#define _IRIS_PHYSX_COLOR_MATCHER_

#include <stddef.h>

#include "iris_physx/color_matcher_vtable.h"

//
// Functions
//

ISTATUS
ColorMatcherAllocate(
    _In_ PCCOLOR_MATCHER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_MATCHER *color_matcher
    );

void
ColorMatcherRetain(
    _In_opt_ PCOLOR_MATCHER color_matcher
    );

void
ColorMatcherRelease(
    _In_opt_ _Post_invalid_ PCOLOR_MATCHER color_matcher
    );

#endif // _IRIS_PHYSX_COLOR_MATCHER_