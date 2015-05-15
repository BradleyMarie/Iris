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
(*PSPECTRUM_TEXTURE_LOOKUP)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCEMISSIVE_LIGHT *EmissiveLight,
    _Out_ PCDIRECT_LIGHT *DirectLight,
    _Out_ PCINDIRECT_LIGHT *IndirectLight
    );

typedef struct _SPECTRUM_TEXTURE_VTABLE {
    PSPECTRUM_TEXTURE_LOOKUP LookupRoutine;
    PFREE_ROUTINE FreeRoutine;
} SPECTRUM_TEXTURE_VTABLE, *PSPECTRUM_TEXTURE_VTABLE;

typedef CONST SPECTRUM_TEXTURE_VTABLE *PCSPECTRUM_TEXTURE_VTABLE;

typedef struct _SPECTRUM_TEXTURE SPECTRUM_TEXTURE, *PSPECTRUM_TEXTURE;
typedef CONST SPECTRUM_TEXTURE *PCSPECTRUM_TEXTURE;

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PSPECTRUM_TEXTURE
SpectrumTextureAllocate(
    _In_ PCSPECTRUM_TEXTURE_VTABLE TextureVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumTextureLookup(
    _In_ PCSPECTRUM_TEXTURE Texture,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCEMISSIVE_LIGHT *EmissiveLight,
    _Out_ PCDIRECT_LIGHT *DirectLight,
    _Out_ PCINDIRECT_LIGHT *IndirectLight
    );

IRISPHYSXAPI
VOID
SpectrumTextureReference(
    _In_opt_ PSPECTRUM_TEXTURE Texture
    );

IRISPHYSXAPI
VOID
SpectrumTextureDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_TEXTURE Texture
    );

#endif // _TEXTURE_IRIS_PHYSX_