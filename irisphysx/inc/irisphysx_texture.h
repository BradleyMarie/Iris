/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_texture.h

Abstract:

    This file contains the definitions for the TEXTURE type.

--*/

#ifndef _TEXTURE_IRIS_PHYSX_
#define _TEXTURE_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PTEXTURE_LOOKUP)(
    _In_ PCVOID Context
    );

typedef struct _TEXTURE_VTABLE {
    PTEXTURE_LOOKUP SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} TEXTURE_VTABLE, *PTEXTURE_VTABLE;

typedef CONST TEXTURE_VTABLE *PCTEXTURE_VTABLE;

typedef struct _TEXTURE TEXTURE, *PTEXTURE;
typedef CONST TEXTURE *PCTEXTURE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PTEXTURE
TextureAllocate(
    _In_ PCTEXTURE_VTABLE TextureVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
TextureSample(
    _In_ PCTEXTURE Texture
    );

IRISPHYSXAPI
VOID
TextureReference(
    _In_opt_ PTEXTURE Texture
    );

IRISPHYSXAPI
VOID
TextureDereference(
    _In_opt_ _Post_invalid_ PTEXTURE Texture
    );

#endif // _TEXTURE_IRIS_PHYSX_