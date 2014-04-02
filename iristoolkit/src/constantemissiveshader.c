/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    constantemissiveshader.c

Abstract:

    This file contains the definitions for the CONSTANT_EMISSIVE_SHADER type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _CONSTANT_EMISSIVE_SHADER {
    EMISSIVE_SHADER EmissiveShaderHeader;
    COLOR3 Color;
} CONSTANT_EMISSIVE_SHADER, *PCONSTANT_EMISSIVE_SHADER;

typedef CONST CONSTANT_EMISSIVE_SHADER *PCCONSTANT_EMISSIVE_SHADER;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ConstantEmissiveShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    PCCONSTANT_EMISSIVE_SHADER ConstantEmissiveShader;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(Emissive != NULL);

    ConstantEmissiveShader = (PCONSTANT_EMISSIVE_SHADER) Context;

    *Emissive = ConstantEmissiveShader->Color;

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC EMISSIVE_SHADER_VTABLE ConstantEmissiveShaderVTable = {
    ConstantEmissiveShaderShade,
    free
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_SHADER
ConstantEmissiveShaderAllocate(
    _In_ PCOLOR3 Color
    )
{
    PCONSTANT_EMISSIVE_SHADER ConstantEmissiveShader;

    if (Color == NULL)
    {
        return NULL;
    }

    ConstantEmissiveShader = (PCONSTANT_EMISSIVE_SHADER) malloc(sizeof(CONSTANT_EMISSIVE_SHADER));

    if (ConstantEmissiveShader == NULL)
    {
        return NULL;
    }

    ConstantEmissiveShader->EmissiveShaderHeader.EmissiveShaderVTable = &ConstantEmissiveShaderVTable;
    ConstantEmissiveShader->Color = *Color;

    return (PEMISSIVE_SHADER) ConstantEmissiveShader;
}