/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bsdf_vtable.h

Abstract:

    The vtable for a BSDF.

--*/

#ifndef _IRIS_PHYSX_BSDF_VTABLE_
#define _IRIS_PHYSX_BSDF_VTABLE_

#include "iris/iris.h"
#include "iris_advanced/iris_advanced.h"
#include "iris_physx/bsdf_sample_type.h"
#include "iris_physx/reflector_compositor.h"

//
// Types
//

typedef
ISTATUS
(*PBSDF_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PBSDF_SAMPLE_DIFFUSE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PBSDF_COMPUTE_DIFFUSE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    );

typedef
ISTATUS
(*PBSDF_COMPUTE_DIFFUSE_WITH_PDF_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 shading_normal,
    _In_ VECTOR3 outgoing,
    _In_ bool transmitted,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ float_t *pdf
    );

typedef struct _BSDF_VTABLE {
    PBSDF_SAMPLE_ROUTINE sample_routine;
    PBSDF_SAMPLE_DIFFUSE_ROUTINE sample_diffuse_routine;
    PBSDF_COMPUTE_DIFFUSE_ROUTINE compute_diffuse_routine;
    PBSDF_COMPUTE_DIFFUSE_WITH_PDF_ROUTINE compute_diffuse_with_pdf_routine;
    PFREE_ROUTINE free_routine;
} BSDF_VTABLE, *PBSDF_VTABLE;

typedef const BSDF_VTABLE *PCBSDF_VTABLE;

#endif // _IRIS_PHYSX_BSDF_VTABLE_