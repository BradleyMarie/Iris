/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    checkerboard.cpp

Abstract:

    This file contains the definitions for the CHECKERBOARD_TEXTURE type.

--*/

#include <iristest.h>

//
// Types
//

typedef struct _CHECKERBOARD_TEXTURE {
    PCEMISSIVE_SHADER EmissiveShaders[2];
    PCDIRECT_SHADER DirectShaders[2];
    PCINDIRECT_SHADER IndirectShaders[2];
    PTRANSLUCENT_SHADER TranslucentShaders[2];
} CHECKERBOARD_TEXTURE, *PCHECKERBOARD_TEXTURE;

typedef CONST CHECKERBOARD_TEXTURE *PCCHECKERBOARD_TEXTURE;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
XZCheckerboardTextureGetShader(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    )
{
    PCHECKERBOARD_TEXTURE CheckerboardTexture;
    FLOAT CheckerboardIndex;
    FLOAT ManhattanDistance;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(TextureShader != NULL);

    CheckerboardTexture = (PCHECKERBOARD_TEXTURE) Context;

    ManhattanDistance = FloorFloat(ModelHitPoint->Z) + 
                        FloorFloat(ModelHitPoint->X);

    CheckerboardIndex = ModFloat(ManhattanDistance, (FLOAT) 2.0);

    if (IsZeroFloat(CheckerboardIndex) == FALSE)
    {
        Status = TextureShaderShadeShader(TextureShader,
                                          CheckerboardTexture->EmissiveShaders[0],
                                          CheckerboardTexture->DirectShaders[0],
                                          CheckerboardTexture->IndirectShaders[0],
                                          CheckerboardTexture->TranslucentShaders[0]);
    }
    else
    {
        Status = TextureShaderShadeShader(TextureShader,
                                          CheckerboardTexture->EmissiveShaders[1],
                                          CheckerboardTexture->DirectShaders[1],
                                          CheckerboardTexture->IndirectShaders[1],
                                          CheckerboardTexture->TranslucentShaders[1]);
    }

    return Status;
}

STATIC
VOID
XZCheckerboardTextureFree(
    _In_ PVOID Context
)
{
    PCHECKERBOARD_TEXTURE CheckerboardTexture;

    ASSERT(Context != NULL);

    CheckerboardTexture = (PCHECKERBOARD_TEXTURE) Context;

    TranslucentShaderDereference(CheckerboardTexture->TranslucentShaders[0]);
    TranslucentShaderDereference(CheckerboardTexture->TranslucentShaders[1]);

    free(Context);
}

//
// Static variables
//

CONST STATIC TEXTURE_VTABLE XZCheckerboardTextureVTable = {
    XZCheckerboardTextureGetShader,
    free
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PTEXTURE
XZCheckerboardTextureAllocate(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader0,
    _In_opt_ PCDIRECT_SHADER DirectShader0,
    _In_opt_ PCINDIRECT_SHADER IndirectShader0,
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader0,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader1,
    _In_opt_ PCDIRECT_SHADER DirectShader1,
    _In_opt_ PCINDIRECT_SHADER IndirectShader1,
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader1
    )
{
    CHECKERBOARD_TEXTURE CheckerboardTexture;
    PTEXTURE Texture;

    if (EmissiveShader0 == NULL &&
        DirectShader0 == NULL &&
        IndirectShader0 == NULL &&
        TranslucentShader0 == NULL &&
        EmissiveShader1 == NULL &&
        DirectShader1 == NULL &&
        IndirectShader1 == NULL &&
        TranslucentShader1 == NULL)
    {
        return NULL;
    }

    CheckerboardTexture.EmissiveShaders[0] = EmissiveShader0;
    CheckerboardTexture.IndirectShaders[0] = IndirectShader0;
    CheckerboardTexture.DirectShaders[0] = DirectShader0;
    CheckerboardTexture.TranslucentShaders[0] = TranslucentShader0;
    CheckerboardTexture.EmissiveShaders[1] = EmissiveShader1;
    CheckerboardTexture.IndirectShaders[1] = IndirectShader1;
    CheckerboardTexture.DirectShaders[1] = DirectShader1;
    CheckerboardTexture.TranslucentShaders[1] = TranslucentShader1;

    Texture = TextureAllocate(&XZCheckerboardTextureVTable,
                              &CheckerboardTexture,
                              sizeof(CHECKERBOARD_TEXTURE),
                              sizeof(PVOID));

    if (Texture != NULL)
    {
        TranslucentShaderReference(TranslucentShader0);
        TranslucentShaderReference(TranslucentShader1);
    }

    return Texture;
}