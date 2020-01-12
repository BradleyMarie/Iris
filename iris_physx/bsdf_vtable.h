/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    bsdf_vtable.h

Abstract:

    The vtable for a BSDF.

--*/

#ifndef _IRIS_PHYSX_BSDF_VTABLE_
#define _IRIS_PHYSX_BSDF_VTABLE_

#include "iris/iris.h"
#include "iris_advanced/iris_advanced.h"
#include "iris_physx/color_cache.h"
#include "iris_physx/reflector_compositor.h"

//
// Types
//

typedef
ISTATUS
(*PBSDF_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PBSDF_COMPUTE_REFLECTANCE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted
    );

typedef
ISTATUS
(*PBSDF_COMPUTE_REFLECTANCE_WITH_PDF_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ float_t *pdf
    );

typedef struct _BSDF_VTABLE {
    PBSDF_SAMPLE_ROUTINE sample_routine;
    PBSDF_COMPUTE_REFLECTANCE_ROUTINE compute_reflectance_routine;
    PBSDF_COMPUTE_REFLECTANCE_WITH_PDF_ROUTINE compute_reflectance_with_pdf_routine;
    PCACHE_COLORS_ROUTINE cache_colors_routine;
    PFREE_ROUTINE free_routine;
} BSDF_VTABLE, *PBSDF_VTABLE;

typedef const BSDF_VTABLE *PCBSDF_VTABLE;

#endif // _IRIS_PHYSX_BSDF_VTABLE_