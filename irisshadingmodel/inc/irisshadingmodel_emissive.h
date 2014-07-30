/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_emissive.h

Abstract:

    This file contains the definitions for the EMISSIVE_SHADER type.

--*/

#ifndef _EMISSIVE_SHADER_IRIS_SHADING_MODEL_
#define _EMISSIVE_SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PEMISSIVE_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    );

typedef struct _EMISSIVE_SHADER_VTABLE {
    PEMISSIVE_SHADING_ROUTINE EmissiveRoutine;
    PFREE_ROUTINE FreeRoutine;
} EMISSIVE_SHADER_VTABLE, *PEMISSIVE_SHADER_VTABLE;

typedef CONST EMISSIVE_SHADER_VTABLE *PCEMISSIVE_SHADER_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PEMISSIVE_SHADER
EmissiveShaderAllocate(
    _In_ PCEMISSIVE_SHADER_VTABLE EmissiveShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
EmissiveShaderShade(
    _In_ PCEMISSIVE_SHADER EmissiveShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    );

IRISSHADINGMODELAPI
VOID
EmissiveShaderReference(
    _In_opt_ PEMISSIVE_SHADER EmissiveShader
    );

IRISSHADINGMODELAPI
VOID
EmissiveShaderDereference(
    _In_opt_ _Post_invalid_ PEMISSIVE_SHADER EmissiveShader
    );

#endif // _EMISSIVE_SHADER_IRIS_SHADING_MODEL_