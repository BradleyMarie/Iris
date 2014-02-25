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
ISTATUS
(*PEMISSIVE_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    );

typedef struct _EMISSIVE_SHADER_VTABLE {
    PEMISSIVE_SHADING_ROUTINE EmissiveRoutine;
    PFREE_ROUTINE FreeRoutine;
} EMISSIVE_SHADER_VTABLE, *PEMISSIVE_SHADER_VTABLE;

typedef CONST EMISSIVE_SHADER_VTABLE *PCEMISSIVE_SHADER_VTABLE;

typedef struct _EMISSIVE_SHADER {
    PCEMISSIVE_SHADER_VTABLE EmissiveShaderVTable;
} EMISSIVE_SHADER, *PEMISSIVE_SHADER;

typedef CONST EMISSIVE_SHADER *PCEMISSIVE_SHADER;

//
// Functions
//

SFORCEINLINE
VOID
EmissiveShaderFree(
    _Pre_maybenull_ _Post_invalid_ PEMISSIVE_SHADER EmissiveShader
    )
{
    if (EmissiveShader == NULL)
    {
        return;
    }

    EmissiveShader->EmissiveShaderVTable->FreeRoutine(EmissiveShader);
}

#endif // _EMISSIVE_SHADER_IRIS_SHADING_MODEL_