/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_normal.h

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#ifndef _NORMAL_IRIS_ADVANCED_
#define _NORMAL_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PNORMAL_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef struct _NORMAL_VTABLE {
    PNORMAL_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    PFREE_ROUTINE FreeRoutine;
    BOOL Prenormalized;
} NORMAL_VTABLE, *PNORMAL_VTABLE;

typedef CONST NORMAL_VTABLE *PCNORMAL_VTABLE;

typedef struct _NORMAL NORMAL, *PNORMAL;
typedef CONST NORMAL *PCNORMAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalAllocate(
    _In_ PCNORMAL_VTABLE NormalVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PNORMAL *Normal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormalAndNormalize(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormalAndTransform(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormalAndTransformAndNormalize(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalPrenormalized(
    _In_ PCNORMAL Normal,
    _Out_ PBOOL Prenormalized
    );

IRISADVANCEDAPI
VOID
NormalReference(
    _In_opt_ PNORMAL Normal
    );

IRISADVANCEDAPI
VOID
NormalDereference(
    _In_opt_ _Post_invalid_ PNORMAL Normal
    );

#endif // _NORMAL_IRIS_ADVANCED_