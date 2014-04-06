/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    constanttexture.c

Abstract:

    This file contains the definitions for the CONSTANT_TEXTURE type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _CONSTANT_TEXTURE {
    TEXTURE TextureHeader;
    SHADER Shader;
} CONSTANT_TEXTURE, *PCONSTANT_TEXTURE;

typedef CONST CONSTANT_TEXTURE *PCCONSTANT_TEXTURE;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
PCSHADER
ConstantTextureGetShader(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData
    )
{
    PCCONSTANT_TEXTURE ConstantTexture;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);

    ConstantTexture = (PCCONSTANT_TEXTURE) Context;

    return &ConstantTexture->Shader;
}

//
// Static variables
//

CONST STATIC TEXTURE_VTABLE ConstantTextureVTable = {
    ConstantTextureGetShader,
    free
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PTEXTURE
ConstantTextureAllocate(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    )
{
    PCONSTANT_TEXTURE ConstantTexture;

    if (EmissiveShader == NULL &&
        DirectShader == NULL &&
        IndirectShader == NULL &&
        TranslucentShader == NULL)
    {
        return NULL;
    }

    ConstantTexture = (PCONSTANT_TEXTURE) malloc(sizeof(CONSTANT_TEXTURE));

    if (ConstantTexture == NULL)
    {
        return NULL;
    }

    ConstantTexture->TextureHeader.TextureVTable = &ConstantTextureVTable;
    ConstantTexture->Shader.EmissiveShader = EmissiveShader;
    ConstantTexture->Shader.IndirectShader = IndirectShader;
    ConstantTexture->Shader.DirectShader = DirectShader;
    ConstantTexture->Shader.TranslucentShader = TranslucentShader;

    return (PTEXTURE) ConstantTexture;
}