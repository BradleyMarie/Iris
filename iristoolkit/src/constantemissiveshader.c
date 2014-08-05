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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    PCCONSTANT_EMISSIVE_SHADER ConstantEmissiveShader;

    ASSERT(Context != NULL);
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
    NULL
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_SHADER
ConstantEmissiveShaderAllocate(
    _In_ COLOR3 Color
    )
{
    CONSTANT_EMISSIVE_SHADER ConstantEmissiveShader;
    PEMISSIVE_SHADER EmissiveShader;

    ConstantEmissiveShader.Color = Color;

    EmissiveShader = EmissiveShaderAllocate(&ConstantEmissiveShaderVTable,
                                            &ConstantEmissiveShader,
                                            sizeof(CONSTANT_EMISSIVE_SHADER),
                                            sizeof(PVOID));

    return EmissiveShader;
}