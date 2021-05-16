/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    light_internal.h

Abstract:

    The internal routines for sampling from a light.

--*/

#ifndef _IRIS_PHYSX_LIGHT_INTERNAL_
#define _IRIS_PHYSX_LIGHT_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/light_vtable.h"

//
// Types
//

struct _LIGHT {
    PCLIGHT_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

#endif // _IRIS_PHYSX_LIGHT_INTERNAL_