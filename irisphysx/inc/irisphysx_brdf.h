/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the PBR_BRDF type.

--*/

#ifndef _PBR_BRDF_IRIS_PHYSX_
#define _PBR_BRDF_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_BRDF_SAMPLE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_BRDF_COMPUTE_REFLECTANCE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_BRDF_COMPUTE_REFLECTANCE_WITH_PDF)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef struct _PBR_BRDF_VTABLE {
    PPBR_BRDF_SAMPLE SampleRoutine;
    PPBR_BRDF_SAMPLE SampleRoutineWithLambertianFalloff;
    PPBR_BRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine;
    PPBR_BRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutineWithLambertianFalloff;
    PPBR_BRDF_COMPUTE_REFLECTANCE_WITH_PDF ComputeReflectanceWithPdfRoutine;
    PPBR_BRDF_COMPUTE_REFLECTANCE_WITH_PDF ComputeReflectanceWithPdfRoutineWithLambertianFalloff;
} PBR_BRDF_VTABLE, *PPBR_BRDF_VTABLE;

typedef CONST PBR_BRDF_VTABLE *PCPBR_BRDF_VTABLE;

typedef struct _PBR_BRDF PBR_BRDF, *PPBR_BRDF;
typedef CONST PBR_BRDF *PCPBR_BRDF;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfSample(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfSampleWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfComputeReflectance(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfComputeReflectanceWithPdf(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

#endif // _PBR_BRDF_IRIS_PHYSX_