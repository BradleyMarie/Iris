/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_indirect.h

Abstract:

    This file contains the definitions for the INDIRECT_MATERIAL type.

--*/

#ifndef _INDIRECT_SHADER_IRIS_SHADING_MODEL_
#define _INDIRECT_SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PINDIRECT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    );

typedef struct _INDIRECT_SHADER_VTABLE {
    PINDIRECT_SHADING_ROUTINE IndirectRoutine;
    PFREE_ROUTINE FreeRoutine;
} INDIRECT_SHADER_VTABLE, *PINDIRECT_SHADER_VTABLE;

typedef CONST INDIRECT_SHADER_VTABLE *PCINDIRECT_SHADER_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PINDIRECT_SHADER
IndirectShaderAllocate(
    _In_ PCINDIRECT_SHADER_VTABLE IndirectShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
IndirectShaderShade(
    _In_ PCINDIRECT_SHADER IndirectShader,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    );

IRISSHADINGMODELAPI
VOID
IndirectShaderReference(
    _In_opt_ PINDIRECT_SHADER IndirectShader
    );

IRISSHADINGMODELAPI
VOID
IndirectShaderDereference(
    _In_opt_ _Post_invalid_ PINDIRECT_SHADER IndirectShader
    );

#endif // _INDIRECT_SHADER_IRIS_SHADING_MODEL_