/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    brdf_internal.h

Abstract:

    The internal routines for sampling from a BRDF.

--*/

#ifndef _IRIS_PHYSX_BRDF_INTERNAL_
#define _IRIS_PHYSX_BRDF_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/brdf_vtable.h"

//
// Types
//

struct _BRDF {
    PCBRDF_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
void
BrdfInitialize(
    _In_ PCBRDF_VTABLE vtable,
    _In_opt_ void *data,
    _Out_ struct _BRDF *brdf
    )
{
    assert(vtable != NULL);
    assert(brdf != NULL);
    
    brdf->vtable = vtable;
    brdf->data = data;
    brdf->reference_count = 1;
}

#endif // _IRIS_PHYSX_BRDF_INTERNAL_