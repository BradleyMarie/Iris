/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    textureshader.c

Abstract:

    This file contains the function definitions for the TEXTURE_SHADER type.

--*/

#include <irisrgbp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
TextureShaderShadeShader(
    _Inout_ PTEXTURE_SHADER TextureShader,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    )
{
    ISTATUS Status;

    if (TextureShader == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

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
                                            EmissiveShader,
                                            DirectShader,
                                            IndirectShader,
                                            TranslucentShader,
                                            TextureShader->SurfaceNormal,
                                            TextureShader->Color);

    return Status;
}