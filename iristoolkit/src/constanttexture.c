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
    PCEMISSIVE_SHADER EmissiveShader;
    PCDIRECT_SHADER DirectShader;
    PCINDIRECT_SHADER IndirectShader;
    PTRANSLUCENT_SHADER TranslucentShader;
} CONSTANT_TEXTURE, *PCONSTANT_TEXTURE;

typedef CONST CONSTANT_TEXTURE *PCCONSTANT_TEXTURE;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ConstantTextureGetShader(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    )
{
    PCCONSTANT_TEXTURE ConstantTexture;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(TextureShader != NULL);

    ConstantTexture = (PCCONSTANT_TEXTURE) Context;

    Status = TextureShaderShadeShader(TextureShader,
                                      ConstantTexture->EmissiveShader,
                                      ConstantTexture->DirectShader,
                                      ConstantTexture->IndirectShader,
                                      ConstantTexture->TranslucentShader);

    return Status;
}

STATIC
VOID
ConstantTextureFree(
    _In_ PVOID Context
    )
{
    PCCONSTANT_TEXTURE ConstantTexture;

    ASSERT(Context != NULL);

    ConstantTexture = (PCCONSTANT_TEXTURE) Context;

    TranslucentShaderDereference(ConstantTexture->TranslucentShader);
}

//
// Static variables
//

CONST STATIC TEXTURE_VTABLE ConstantTextureVTable = {
    ConstantTextureGetShader,
    ConstantTextureFree
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
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader
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
    ConstantTexture->EmissiveShader = EmissiveShader;
    ConstantTexture->IndirectShader = IndirectShader;
    ConstantTexture->DirectShader = DirectShader;
    ConstantTexture->TranslucentShader = TranslucentShader;

    TranslucentShaderReference(TranslucentShader);

    return (PTEXTURE) ConstantTexture;
}