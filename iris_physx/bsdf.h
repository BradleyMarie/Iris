/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    bsdf.h

Abstract:

    The interface for an object representing a BSDF.

--*/

#ifndef _IRIS_BSDF_
#define _IRIS_BSDF_

#include "iris_physx/bsdf_vtable.h"

//
// Types
//

typedef struct _BSDF BSDF, *PBSDF;
typedef const BSDF *PCBSDF;

//
// Functions
//

ISTATUS
BsdfAllocate(
    _In_ PCBSDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PBSDF *bsdf
    );

ISTATUS
BsdfSample(
    _In_ PCBSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

ISTATUS
BsdfComputeReflectance(
    _In_ PCBSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted
    );

ISTATUS
BsdfComputeReflectanceWithPdf(
    _In_ PCBSDF bsdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ float_t *pdf
    );

ISTATUS
BsdfCacheColors(
    _In_opt_ PCBSDF bsdf,
    _In_ PCOLOR_CACHE color_cache
    );

void
BsdfRetain(
    _In_opt_ PBSDF bsdf
    );

void
BsdfRelease(
    _In_opt_ _Post_invalid_ PBSDF bsdf
    );

#endif // _IRIS_BSDF_