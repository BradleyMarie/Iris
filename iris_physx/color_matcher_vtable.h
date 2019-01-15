/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    color_matcher_vtable.h

Abstract:

    The vtable for a color matcher.

--*/

#ifndef _IRIS_PHYSX_COLOR_MATCHER_VTABLE_
#define _IRIS_PHYSX_COLOR_MATCHER_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef struct _COLOR_MATCHER COLOR_MATCHER, *PCOLOR_MATCHER;
typedef const COLOR_MATCHER *PCCOLOR_MATCHER;

typedef
ISTATUS
(*PCOLOR_MATCHER_COMPUTE_ROUTINE)(
    _In_ const void *context,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    );

typedef struct _COLOR_MATCHER_VTABLE {
    PCOLOR_MATCHER_COMPUTE_ROUTINE compute_routine;
    PFREE_ROUTINE free_routine;
} COLOR_MATCHER_VTABLE, *PCOLOR_MATCHER_VTABLE;

typedef const COLOR_MATCHER_VTABLE *PCCOLOR_MATCHER_VTABLE;

#endif // _IRIS_PHYSX_COLOR_MATCHER_VTABLE_