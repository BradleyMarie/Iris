/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_texture.h

Abstract:

    This file contains the definitions for the TEXTURE type.

--*/

#ifndef _TEXTURE_IRIS_SHADING_MODEL_
#define _TEXTURE_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Ret_maybenull_
PCSHADER
(*PGET_SHADER_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData
    );

typedef struct _TEXTURE_VTABLE {
    PGET_SHADER_ROUTINE GetShaderRoutine;
    PFREE_ROUTINE FreeRoutine;
} TEXTURE_VTABLE, *PTEXTURE_VTABLE;

typedef CONST TEXTURE_VTABLE *PCTEXTURE_VTABLE;

typedef struct _TEXTURE {
    PCTEXTURE_VTABLE TextureVTable;
} TEXTURE, *PTEXTURE;

typedef CONST TEXTURE *PCTEXTURE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
PCSHADER
TextureGetShader(
    _In_ PCTEXTURE Texture,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData
    )
{
    PCSHADER Shader;

    ASSERT(Texture != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);

    Shader = Texture->TextureVTable->GetShaderRoutine(Texture,
                                                      WorldHitPoint,
                                                      ModelHitPoint,
                                                      AdditionalData);

    return Shader;
}

SFORCEINLINE
VOID
TextureFree(
    _Pre_maybenull_ _Post_invalid_ PTEXTURE Texture
    )
{
    if (Texture == NULL)
    {
        return;
    }

    Texture->TextureVTable->FreeRoutine(Texture);
}

#endif // _TEXTURE_IRIS_SHADING_MODEL_