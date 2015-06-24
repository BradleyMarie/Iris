/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the BRDF_SHADER type.

--*/

#ifndef _BRDF_SHADER_IRIS_PHYSX_
#define _BRDF_SHADER_IRIS_PHYSX_

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
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
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
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBRDF_APROXIMATE_ALL_OUTGOING)(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _In_ UINT32 NumberOfSamples,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBRDF_APROXIMATE_ALL_DIRECTIONS)(
    _In_ PCVOID Context,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _In_ UINT32 NumberOfSamples,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

typedef struct _BRDF_VTABLE {
    PBRDF_SAMPLE SampleRoutine;
    PBRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine;
    PBRDF_APROXIMATE_ALL_OUTGOING AproximateAllOutgoing;
    PBRDF_APROXIMATE_ALL_DIRECTIONS AproximateAllDirections;
    PFREE_ROUTINE FreeRoutine;
} BRDF_VTABLE, *PBRDF_VTABLE;

typedef CONST BRDF_VTABLE *PCBRDF_VTABLE;

typedef struct _BRDF BRDF, *PBRDF;
typedef CONST BRDF *PCBRDF;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PBRDF
BrdfAllocate(
    _In_ PCBRDF_VTABLE BrdfVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfSample(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
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
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfAproximateAllOutgoing(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _In_ UINT32 NumberOfSamples,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfAproximateAllDirections(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _In_ UINT32 NumberOfSamples,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    );

IRISPHYSXAPI
VOID
BrdfReference(
    _In_opt_ PBRDF Brdf
    );

IRISPHYSXAPI
VOID
BrdfDereference(
    _In_opt_ _Post_invalid_ PBRDF Brdf
    );

#endif // _BRDF_SHADER_IRIS_PHYSX_