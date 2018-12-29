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
// Types
//

typedef struct _COLOR_MATCHER COLOR_MATCHER, *PCOLOR_MATCHER;
typedef const COLOR_MATCHER *PCCOLOR_MATCHER;

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

ISTATUS
ColorMatcherComputeColor(
    _In_ PCCOLOR_MATCHER color_matcher,
    _Out_ PCOLOR3 color
    );

ISTATUS
ColorMatcherClear(
    _In_ PCOLOR_MATCHER color_matcher
    );

void
ColorMatcherFree(
    _In_opt_ _Post_invalid_ PCOLOR_MATCHER color_matcher
    );

#endif // _IRIS_PHYSX_COLOR_MATCHER_