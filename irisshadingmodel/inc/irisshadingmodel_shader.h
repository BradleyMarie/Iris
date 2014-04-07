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

#endif // _SHADER_IRIS_SHADING_MODEL_