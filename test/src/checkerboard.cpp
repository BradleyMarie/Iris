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
    TEXTURE TextureHeader;
    SHADER Shaders[2];
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

    if (CheckerboardIndex > (FLOAT) 0.0)
    {
        Status = TextureShaderShadeShader(TextureShader,
                                          &CheckerboardTexture->Shaders[0]);
    }
    else
    {
        Status = TextureShaderShadeShader(TextureShader,
                                          &CheckerboardTexture->Shaders[1]);   
    }

    return Status;
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
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader0,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader1,
    _In_opt_ PCDIRECT_SHADER DirectShader1,
    _In_opt_ PCINDIRECT_SHADER IndirectShader1,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader1
    )
{
    PCHECKERBOARD_TEXTURE CheckerboardTexture;

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

    CheckerboardTexture = (PCHECKERBOARD_TEXTURE) malloc(sizeof(CHECKERBOARD_TEXTURE));

    if (CheckerboardTexture == NULL)
    {
        return NULL;
    }

    CheckerboardTexture->TextureHeader.TextureVTable = &XZCheckerboardTextureVTable;
    CheckerboardTexture->Shaders[0].EmissiveShader = EmissiveShader0;
    CheckerboardTexture->Shaders[0].IndirectShader = IndirectShader0;
    CheckerboardTexture->Shaders[0].DirectShader = DirectShader0;
    CheckerboardTexture->Shaders[0].TranslucentShader = TranslucentShader0;
    CheckerboardTexture->Shaders[1].EmissiveShader = EmissiveShader1;
    CheckerboardTexture->Shaders[1].IndirectShader = IndirectShader1;
    CheckerboardTexture->Shaders[1].DirectShader = DirectShader1;
    CheckerboardTexture->Shaders[1].TranslucentShader = TranslucentShader1;

    return (PTEXTURE) CheckerboardTexture;
}