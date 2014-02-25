/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_translucent.h

Abstract:

    This file contains the definitions for the TRANSLUCENT_MATERIAL type.

--*/

#ifndef _TRANSLUCENT_SHADER_IRIS_SHADING_MODEL_
#define _TRANSLUCENT_SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
ISTATUS
(*PTRANSLUCENT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    );

typedef struct _TRANSLUCENT_SHADER_VTABLE {
    PTRANSLUCENT_SHADING_ROUTINE TranslucentRoutine;
    PFREE_ROUTINE FreeRoutine;
} TRANSLUCENT_SHADER_VTABLE, *PTRANSLUCENT_SHADER_VTABLE;

typedef CONST TRANSLUCENT_SHADER_VTABLE *PCTRANSLUCENT_SHADER_VTABLE;

typedef struct _TRANSLUCENT_SHADER {
    PCTRANSLUCENT_SHADER_VTABLE TranslucentShaderVTable;
} TRANSLUCENT_SHADER, *PTRANSLUCENT_SHADER;

typedef CONST TRANSLUCENT_SHADER *PCTRANSLUCENT_SHADER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TranslucentShaderShade(
    _In_ PCTRANSLUCENT_SHADER TranslucentShader,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    )
{
    ISTATUS Status;

    ASSERT(TranslucentShader != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(Alpha != NULL);

    Status = TranslucentShader->TranslucentShaderVTable->TranslucentRoutine(TranslucentShader,
                                                                            WorldHitPoint,
                                                                            ModelHitPoint,
                                                                            AdditionalData,
                                                                            Alpha);

    return Status;
}

SFORCEINLINE
VOID
TranslucentShaderFree(
    _Pre_maybenull_ _Post_invalid_ PTRANSLUCENT_SHADER TranslucentShader
    )
{
    if (TranslucentShader == NULL)
    {
        return;
    }

    TranslucentShader->TranslucentShaderVTable->FreeRoutine(TranslucentShader);
}

#endif // _TRANSLUCENT_SHADER_IRIS_SHADING_MODEL_