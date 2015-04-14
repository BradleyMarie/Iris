/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_texture.h

Abstract:

    This file contains the definitions for the TEXTURE type.

--*/

#ifndef _TEXTURE_IRIS_RGB_
#define _TEXTURE_IRIS_RGB_

#include <irisrgb.h>

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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    );

typedef struct _TEXTURE_VTABLE {
    PTEXTURE_SHADE_ROUTINE TextureShadeRoutine;
    PFREE_ROUTINE FreeRoutine;
} TEXTURE_VTABLE, *PTEXTURE_VTABLE;

typedef CONST TEXTURE_VTABLE *PCTEXTURE_VTABLE;

typedef struct _TEXTURE TEXTURE, *PTEXTURE;
typedef CONST TEXTURE *PCTEXTURE;

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
TextureShaderShadeShader(
    _Inout_ PTEXTURE_SHADER TextureShader,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    );

_Check_return_
_Ret_maybenull_
IRISRGBAPI
PTEXTURE
TextureAllocate(
    _In_ PCTEXTURE_VTABLE TextureVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
TextureShade(
    _In_ PCTEXTURE Texture,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    );

IRISRGBAPI
VOID
TextureReference(
    _In_opt_ PTEXTURE Texture
    );

IRISRGBAPI
VOID
TextureDereference(
    _In_opt_ _Post_invalid_ PTEXTURE Texture
    );

#endif // _TEXTURE_IRIS_RGB_