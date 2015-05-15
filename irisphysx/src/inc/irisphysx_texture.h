/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_texture.h

Abstract:

    This file contains the internal definitions for the SPECTRUM_TEXTURE type.

--*/

#ifndef _TEXTURE_IRIS_PHYSX_INTERNAL_
#define _TEXTURE_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_TEXTURE {
    PCSPECTRUM_TEXTURE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

#ifdef _IRIS_PHYSX_EXPORT_SPECTRUM_TEXTURE_ROUTINES_
#define SpectrumTextureLookup(Texture, WorldHitPoint, ModelHitPoint, AdditionalData, EmimssiveLight, DirectLight, IndirectLight) \
        StaticSpectrumTextureLookup(Texture, WorldHitPoint, ModelHitPoint, AdditionalData, EmimssiveLight, DirectLight, IndirectLight)
#endif

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumTextureLookup(
    _In_ PCSPECTRUM_TEXTURE Texture,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCEMISSIVE_LIGHT *EmissiveLight,
    _Out_ PCDIRECT_LIGHT *DirectLight,
    _Out_ PCINDIRECT_LIGHT *IndirectLight
    )
{
    ISTATUS Status;
    
    ASSERT(Texture != NULL);
    ASSERT(EmissiveLight != NULL);
    ASSERT(DirectLight != NULL);
    ASSERT(IndirectLight != NULL);
    
    Status = Texture->VTable->LookupRoutine(Texture->Data,
                                            WorldHitPoint,
                                            ModelHitPoint,
                                            AdditionalData,
                                            EmissiveLight,
                                            DirectLight,
                                            IndirectLight);
                                           
    return Status;
}

#ifdef _IRIS_PHYSX_EXPORT_SPECTRUM_TEXTURE_ROUTINES_
#undef SpectrumTextureLookup
#endif

#endif // _TEXTURE_IRIS_PHYSX_INTERNAL_