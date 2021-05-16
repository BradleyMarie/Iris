/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bsdf_internal.h

Abstract:

    The internal routines for sampling from a BSDF.

--*/

#ifndef _IRIS_PHYSX_BSDF_INTERNAL_
#define _IRIS_PHYSX_BSDF_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/bsdf_vtable.h"

//
// Types
//

struct _BSDF {
    PCBSDF_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
void
BsdfInitialize(
    _In_ PCBSDF_VTABLE vtable,
    _In_opt_ void *data,
    _Out_ struct _BSDF *bsdf
    )
{
    assert(vtable != NULL);
    assert(bsdf != NULL);
    
    bsdf->vtable = vtable;
    bsdf->data = data;
    bsdf->reference_count = 1;
}

#endif // _IRIS_PHYSX_BSDF_INTERNAL_