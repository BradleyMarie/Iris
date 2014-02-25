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

typedef struct _SHADER {
    PEMISSIVE_SHADER EmissiveShader;
    PDIRECT_SHADER DirectShader;
    PINDIRECT_SHADER IndirectShader;
    PTRANSLUCENT_SHADER TranslucentShader;
    PFREE_ROUTINE FreeRoutine;
} SHADER, *PSHADER;

typedef CONST SHADER *PCSHADER;

//
// Functions
//

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

    Shader->FreeRoutine(Shader);
}

#endif // _SHADER_IRIS_SHADING_MODEL_