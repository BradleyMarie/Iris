/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_normal.h

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#ifndef _NORMAL_IRIS_SHADING_MODEL_
#define _NORMAL_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PNORMAL_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
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
_Ret_maybenull_
IRISSHADINGMODELAPI
PNORMAL
NormalAllocate(
    _In_ PCNORMAL_VTABLE TranslucentShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
NormalPrenormalized(
    _In_ PCNORMAL Normal,
    _Out_ PBOOL Prenormalized
    );

IRISSHADINGMODELAPI
VOID
NormalReference(
    _In_opt_ PNORMAL Normal
    );

IRISSHADINGMODELAPI
VOID
NormalDereference(
    _In_opt_ _Post_invalid_ PNORMAL Normal
    );

#endif // _NORMAL_IRIS_SHADING_MODEL_