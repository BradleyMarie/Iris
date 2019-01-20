/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    brdf.h

Abstract:

    The interface for an object representing a BRDF.

--*/

#ifndef _IRIS_BRDF_
#define _IRIS_BRDF_

#include "iris_physx/brdf_vtable.h"

//
// Types
//

typedef struct _BRDF BRDF, *PBRDF;
typedef const BRDF *PCBRDF;

//
// Functions
//

ISTATUS
BrdfAllocate(
    _In_ PCBRDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PBRDF *brdf
    );

ISTATUS
BrdfSample(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

ISTATUS
BrdfComputeReflectance(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector
    );

ISTATUS
BrdfComputeReflectanceWithPdf(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    );

void
BrdfRetain(
    _In_opt_ PBRDF brdf
    );

void
BrdfRelease(
    _In_opt_ _Post_invalid_ PBRDF brdf
    );

#endif // _IRIS_BRDF_