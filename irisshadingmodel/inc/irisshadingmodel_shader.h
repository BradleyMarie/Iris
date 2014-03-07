/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_shader.h

Abstract:

    This file contains the definitions for the SHADER type.

--*/

#ifndef _SHADER_IRIS_SHADING_MODEL_
#define _SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

struct _SHADER {
    PCEMISSIVE_SHADER EmissiveShader;
    PCDIRECT_SHADER DirectShader;
    PCINDIRECT_SHADER IndirectShader;
    PCTRANSLUCENT_SHADER TranslucentShader;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSHADER
ShaderAllocate(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    )
{
    PSHADER Shader;

    Shader = malloc(sizeof(SHADER));

    if (Shader == NULL)
    {
        return NULL;
    }

    Shader->EmissiveShader = EmissiveShader;
    Shader->DirectShader = DirectShader;
    Shader->IndirectShader = IndirectShader;
    Shader->TranslucentShader = TranslucentShader;

    return NULL;
}

SFORCEINLINE
VOID
ShaderFree(
    _Pre_maybenull_ _Post_invalid_ PSHADER Shader
    )
{
    if (Shader == NULL)
    {
        return;
    }

    free(Shader);
}

#endif // _SHADER_IRIS_SHADING_MODEL_