/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    texture.c

Abstract:

    This file contains the function definitions for the TEXTURE type.

--*/

#include <irisrgbp.h>

//
// Types
//

struct _TEXTURE {
    PCTEXTURE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PTEXTURE
TextureAllocate(
    _In_ PCTEXTURE_VTABLE TextureVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PTEXTURE Texture;

    if (TextureVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(TEXTURE),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Texture = (PTEXTURE) HeaderAllocation;

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
TextureShade(
    _In_ PCTEXTURE Texture,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PTEXTURE_SHADER TextureShader
    )
{
    ISTATUS Status;

    if (Texture == NULL ||
        TextureShader == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = Texture->VTable->TextureShadeRoutine(Texture->Data,
                                                  WorldHitPoint,
                                                  ModelHitPoint,
                                                  AdditionalData,
                                                  TextureShader);

    return Status;
}

VOID
TextureReference(
    _In_opt_ PTEXTURE Texture
    )
{
    if (Texture == NULL)
    {
        return;
    }

    Texture->ReferenceCount += 1;
}

VOID
TextureDereference(
    _In_opt_ _Post_invalid_ PTEXTURE Texture
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