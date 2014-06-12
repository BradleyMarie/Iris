/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_emissive.h

Abstract:

    This file contains the definitions for the EMISSIVE_MATERIAL type.

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
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    );

typedef struct _EMISSIVE_SHADER_VTABLE {
    PEMISSIVE_SHADING_ROUTINE EmissiveRoutine;
    PFREE_ROUTINE FreeRoutine;
} EMISSIVE_SHADER_VTABLE, *PEMISSIVE_SHADER_VTABLE;

typedef CONST EMISSIVE_SHADER_VTABLE *PCEMISSIVE_SHADER_VTABLE;

struct _EMISSIVE_SHADER {
    PCEMISSIVE_SHADER_VTABLE EmissiveShaderVTable;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
EmissiveShaderShade(
    _In_ PCEMISSIVE_SHADER EmissiveShader,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    ISTATUS Status;

    ASSERT(EmissiveShader != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(Emissive != NULL);

    Status = EmissiveShader->EmissiveShaderVTable->EmissiveRoutine(EmissiveShader,
                                                                   WorldHitPoint,
                                                                   ModelHitPoint,
                                                                   AdditionalData,
                                                                   Emissive);

    return Status;
}

SFORCEINLINE
VOID
EmissiveShaderFree(
    _In_opt_ _Post_invalid_ PEMISSIVE_SHADER EmissiveShader
    )
{
    if (EmissiveShader == NULL)
    {
        return;
    }

    EmissiveShader->EmissiveShaderVTable->FreeRoutine(EmissiveShader);
}

#endif // _EMISSIVE_SHADER_IRIS_SHADING_MODEL_