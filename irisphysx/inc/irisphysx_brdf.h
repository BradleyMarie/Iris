/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the BRDF type.

--*/

#ifndef _BRDF_IRIS_PHYSX_
#define _BRDF_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBRDF_SAMPLE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBRDF_COMPUTE_REFLECTANCE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBRDF_COMPUTE_REFLECTANCE_WITH_PDF)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef struct _BRDF_VTABLE {
    PBRDF_SAMPLE SampleRoutine;
    PBRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine;
    PBRDF_COMPUTE_REFLECTANCE_WITH_PDF ComputeReflectanceWithPdfRoutine;
} BRDF_VTABLE, *PBRDF_VTABLE;

typedef CONST BRDF_VTABLE *PCBRDF_VTABLE;

typedef struct _BRDF BRDF, *PBRDF;
typedef CONST BRDF *PCBRDF;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfSample(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfComputeReflectance(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfComputeReflectanceWithPdf(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

#endif // _BRDF_IRIS_PHYSX_