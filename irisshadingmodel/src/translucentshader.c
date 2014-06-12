/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    translucentshader.c

Abstract:

    This file contains the definitions for the TRANSLUCENT_SHADER type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _TRANSLUCENT_SHADER {
    PCTRANSLUCENT_SHADER_VTABLE TranslucentShaderVTable;
    SIZE_T DataBeginOffset;
    SIZE_T ReferenceCount;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PTRANSLUCENT_SHADER
TranslucentShaderAllocate(
    _In_ PCTRANSLUCENT_SHADER_VTABLE TranslucentShaderVTable,
    _Field_size_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PTRANSLUCENT_SHADER TranslucentShader;
    SIZE_T DataBeginOffset;
    PVOID DataBeginAddress;
    SIZE_T AllocationSize;
    PVOID Allocation;
    ISTATUS Status;

    if (TranslucentShaderVTable == NULL ||
        Data == NULL ||
        DataSizeInBytes == 0 ||
        DataAlignment == 0)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(sizeof(TRANSLUCENT_SHADER),
                             DataAlignment,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(AllocationSize,
                             DataSizeInBytes,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Allocation = malloc(AllocationSize);

    if (Allocation == NULL)
    {
        return NULL;
    }

    DataBeginOffset = ((UINT_PTR) Allocation + DataAlignment) % DataAlignment - (UINT_PTR) Allocation;
    DataBeginAddress = (PUINT8) Allocation + DataBeginOffset;

    TranslucentShader = (PTRANSLUCENT_SHADER) Allocation;

    TranslucentShader->TranslucentShaderVTable = TranslucentShaderVTable;
    TranslucentShader->ReferenceCount = 1;
    TranslucentShader->DataBeginOffset = (PUINT8) Allocation - (PUINT8) DataBeginAddress;
    memcpy(DataBeginAddress, Data, DataSizeInBytes);

    return TranslucentShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
TranslucentShaderShade(
    _In_ PCTRANSLUCENT_SHADER TranslucentShader,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    )
{
    PCVOID DataBeginAddress;
    ISTATUS Status;

    if (TranslucentShader == NULL ||
        WorldHitPoint == NULL ||
        ModelHitPoint == NULL ||
        Alpha == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    DataBeginAddress = (PCUINT8) TranslucentShader +
                       TranslucentShader->DataBeginOffset;

    Status = TranslucentShader->TranslucentShaderVTable->TranslucentRoutine(DataBeginAddress,
                                                                            WorldHitPoint,
                                                                            ModelHitPoint,
                                                                            AdditionalData,
                                                                            Alpha);

    return Status;
}

VOID
TranslucentShaderReference(
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader
    )
{
    if (TranslucentShader == NULL)
    {
        return;
    }

    TranslucentShader->ReferenceCount += 1;
}

VOID
TranslucentShaderDereference(
    _Pre_maybenull_ _Post_invalid_ PTRANSLUCENT_SHADER TranslucentShader
    )
{
    PFREE_ROUTINE FreeRoutine;
    PVOID DataBeginAddress;

    if (TranslucentShader == NULL)
    {
        return;
    }

    TranslucentShader->ReferenceCount -= 1;

    if (TranslucentShader->ReferenceCount == 0)
    {
        FreeRoutine = TranslucentShader->TranslucentShaderVTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            DataBeginAddress = (PUINT8) TranslucentShader +
                               TranslucentShader->DataBeginOffset;

            FreeRoutine(DataBeginAddress);
        }

        free(TranslucentShader);
    }
}