/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    brdf.h

Abstract:

    The interface for an object representing a brdf.

--*/

#ifndef _IRIS_BRDF_
#define _IRIS_BRDF_

#include "iris/vector.h"
#include "iris_advanced/random.h"
#include "iris_spectrum/reflector_allocator.h"

//
// Types
//

typedef
ISTATUS
(*PBRDF_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PBRDF_COMPUTE_REFLECTANCE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector
    );

typedef
ISTATUS
(*PBRDF_COMPUTE_REFLECTANCE_WITH_PDF_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    );

typedef struct _BRDF_VTABLE {
    PBRDF_SAMPLE_ROUTINE sample_routine;
    PBRDF_COMPUTE_REFLECTANCE_ROUTINE compute_reflectance_routine;
    PBRDF_COMPUTE_REFLECTANCE_WITH_PDF_ROUTINE compute_reflectance_with_pdf_routine;
    PFREE_ROUTINE free_routine;
} BRDF_VTABLE, *PBRDF_VTABLE;

typedef const BRDF_VTABLE *PCBRDF_VTABLE;

typedef struct _BRDF BRDF, *PBRDF;
typedef const BRDF *PCBRDF;

//
// Functions
//

//IRISPHYSXAPI
ISTATUS
BrdfAllocate(
    _In_ PCBRDF_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PBRDF *brdf
    );

//IRISPHYSXAPI
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

//IRISPHYSXAPI
ISTATUS
BrdfComputeReflectance(
    _In_ PCBRDF brdf,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Out_ PCREFLECTOR *reflector
    );

//IRISPHYSXAPI
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

//IRISPHYSXAPI
void
BrdfRetain(
    _In_opt_ PBRDF brdf
    );

//IRISPHYSXAPI
void
BrdfRelease(
    _In_opt_ _Post_invalid_ PBRDF brdf
    );

#endif // _IRIS_BRDF_