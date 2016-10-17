/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the PHYSX_BRDF type.

--*/

#ifndef _PHYSX_BRDF_IRIS_PHYSX_
#define _PHYSX_BRDF_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_BRDF_SAMPLE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
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
(*PPHYSX_BRDF_COMPUTE_REFLECTANCE)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_BRDF_COMPUTE_REFLECTANCE_WITH_PDF)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef struct _PHYSX_BRDF_VTABLE {
    PPHYSX_BRDF_SAMPLE SampleRoutine;
    PPHYSX_BRDF_SAMPLE SampleRoutineWithLambertianFalloff;
    PPHYSX_BRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine;
    PPHYSX_BRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutineWithLambertianFalloff;
    PPHYSX_BRDF_COMPUTE_REFLECTANCE_WITH_PDF ComputeReflectanceWithPdfRoutine;
    PPHYSX_BRDF_COMPUTE_REFLECTANCE_WITH_PDF ComputeReflectanceWithPdfRoutineWithLambertianFalloff;
    PFREE_ROUTINE FreeRoutine;
} PHYSX_BRDF_VTABLE, *PPHYSX_BRDF_VTABLE;

typedef CONST PHYSX_BRDF_VTABLE *PCPHYSX_BRDF_VTABLE;

typedef struct _PHYSX_BRDF PHYSX_BRDF, *PPHYSX_BRDF;
typedef CONST PHYSX_BRDF *PCPHYSX_BRDF;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfAllocate(
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_BRDF *Brdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfSample(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
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
PhysxBrdfSampleWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
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
PhysxBrdfComputeReflectance(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfComputeReflectanceWithPdf(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

IRISPHYSXAPI
VOID
PhysxBrdfRetain(
    _In_opt_ PPHYSX_BRDF Brdf
    );

IRISPHYSXAPI
VOID
PhysxBrdfRelease(
    _In_opt_ _Post_invalid_ PPHYSX_BRDF Brdf
    );

#endif // _PHYSX_BRDF_IRIS_PHYSX_