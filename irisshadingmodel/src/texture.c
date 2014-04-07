/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    texture.c

Abstract:

    This file contains the function definitions for the TEXTURE type.

--*/

#include <irisshadingmodelp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
TextureShaderShadeShader(
    _Inout_ PTEXTURE_SHADER TextureShader,
    _In_ PCSHADER Shader
    )
{
    ISTATUS Status;

    ASSERT(TextureShader != NULL);
    ASSERT(Shader != NULL);

    Status = TextureShader->ShadeRayRoutine(TextureShader->Context,
                                            TextureShader->NextRayShader,
                                            TextureShader->CurrentDepth,
                                            TextureShader->Distance,
                                            TextureShader->WorldViewer,
                                            TextureShader->WorldHit,
                                            TextureShader->ModelViewer,
                                            TextureShader->ModelHit,
                                            TextureShader->ModelToWorld,
                                            TextureShader->AdditionalData,
                                            Shader,
                                            TextureShader->SurfaceNormal,
                                            TextureShader->Color);

    return Status;
}