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
    PEMISSIVE_SHADER EmissiveShader;
    PDIRECT_SHADER DirectShader;
    PINDIRECT_SHADER IndirectShader;
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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    )
{
    PCCONSTANT_TEXTURE ConstantTexture;
    ISTATUS Status;

    ASSERT(Context != NULL);
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
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCCONSTANT_TEXTURE ConstantTexture;

    ASSERT(Context != NULL);

    ConstantTexture = (PCCONSTANT_TEXTURE) Context;

    EmissiveShaderDereference(ConstantTexture->EmissiveShader);
    DirectShaderDereference(ConstantTexture->DirectShader);
    IndirectShaderDereference(ConstantTexture->IndirectShader);
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
    _In_opt_ PEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PDIRECT_SHADER DirectShader,
    _In_opt_ PINDIRECT_SHADER IndirectShader,
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader
    )
{
    CONSTANT_TEXTURE ConstantTexture;
    PTEXTURE Texture;

    if (EmissiveShader == NULL &&
        DirectShader == NULL &&
        IndirectShader == NULL &&
        TranslucentShader == NULL)
    {
        return NULL;
    }

    ConstantTexture.EmissiveShader = EmissiveShader;
    ConstantTexture.IndirectShader = IndirectShader;
    ConstantTexture.DirectShader = DirectShader;
    ConstantTexture.TranslucentShader = TranslucentShader;

    Texture = TextureAllocate(&ConstantTextureVTable,
                              &ConstantTexture,
                              sizeof(CONSTANT_TEXTURE),
                              sizeof(PVOID));

    if (Texture != NULL)
    {
        EmissiveShaderReference(EmissiveShader);
        DirectShaderReference(DirectShader);
        IndirectShaderReference(IndirectShader);
        TranslucentShaderReference(TranslucentShader);
    }
    
    return Texture;
}