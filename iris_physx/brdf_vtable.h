/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    brdf_vtable.h

Abstract:

    The vtable for a BRDF.

--*/

#ifndef _IRIS_PHYSX_BRDF_VTABLE_
#define _IRIS_PHYSX_BRDF_VTABLE_

#include "iris/iris.h"
#include "iris_advanced/iris_advanced.h"
#include "iris_physx/reflector_allocator.h"

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

#endif // _IRIS_PHYSX_BRDF_VTABLE_