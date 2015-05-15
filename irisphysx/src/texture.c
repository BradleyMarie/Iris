/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    texture.c

Abstract:

    This file contains the definitions for the SPECTRUM_TEXTURE type.

--*/

#define _IRIS_PHYSX_EXPORT_SPECTRUM_TEXTURE_ROUTINES_
#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSPECTRUM_TEXTURE
SpectrumTextureAllocate(
    _In_ PCSPECTRUM_TEXTURE_VTABLE TextureVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSPECTRUM_TEXTURE Texture;

    if (TextureVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(SPECTRUM_TEXTURE),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Texture = (PSPECTRUM_TEXTURE) HeaderAllocation;

    Texture->VTable = TextureVTable;
    Texture->Data = DataAllocation;
    Texture->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Texture;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
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
    
    if (Texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (EmissiveLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    if (DirectLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (IndirectLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = StaticSpectrumTextureLookup(Texture,
                                         WorldHitPoint,
                                         ModelHitPoint,
                                         AdditionalData,
                                         EmissiveLight,
                                         DirectLight,
                                         IndirectLight);
                                         
    return Status;
}

VOID
SpectrumTextureReference(
    _In_opt_ PSPECTRUM_TEXTURE Texture
    )
{
    if (Texture == NULL)
    {
        return;
    }

    Texture->ReferenceCount += 1;
}

VOID
SpectrumTextureDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_TEXTURE Texture
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Texture == NULL)
    {
        return;
    }

    Texture->ReferenceCount -= 1;

    if (Texture->ReferenceCount == 0)
    {
        FreeRoutine = Texture->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Texture->Data);
        }

        IrisAlignedFree(Texture);
    }
}