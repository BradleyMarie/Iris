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

typedef struct _TEXTURE_SHADER TEXTURE_SHADER, *PTEXTURE_SHADER;
typedef CONST TEXTURE_SHADER *PCTEXTURE_SHADER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PTEXTURE_SHADE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    );

typedef struct _TEXTURE_VTABLE {
    PTEXTURE_SHADE_ROUTINE TextureShadeRoutine;
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
ISTATUS
TextureShade(
    _In_ PCTEXTURE Texture,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    )
{
    ISTATUS Status;

    ASSERT(Texture != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(TextureShader != NULL);

    Status = Texture->TextureVTable->TextureShadeRoutine(Texture,
                                                         WorldHitPoint,
                                                         ModelHitPoint,
                                                         AdditionalData,
                                                         TextureShader);

    return Status;
}

SFORCEINLINE
VOID
TextureFree(
    _In_opt_ _Post_invalid_ PTEXTURE Texture
    )
{
    if (Texture == NULL)
    {
        return;
    }

    Texture->TextureVTable->FreeRoutine(Texture);
}

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
TextureShaderShadeShader(
    _Inout_ PTEXTURE_SHADER TextureShader,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    );

#endif // _TEXTURE_IRIS_SHADING_MODEL_