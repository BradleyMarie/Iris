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
    PCTRANSLUCENT_SHADER_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PTRANSLUCENT_SHADER
TranslucentShaderAllocate(
    _In_ PCTRANSLUCENT_SHADER_VTABLE TranslucentShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PTRANSLUCENT_SHADER TranslucentShader;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (TranslucentShaderVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(TRANSLUCENT_SHADER),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    TranslucentShader = (PTRANSLUCENT_SHADER) HeaderAllocation;

    TranslucentShader->VTable = TranslucentShaderVTable;
    TranslucentShader->Data = DataAllocation;
    TranslucentShader->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return TranslucentShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
TranslucentShaderShade(
    _In_ PCTRANSLUCENT_SHADER TranslucentShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    )
{
    ISTATUS Status;

    if (TranslucentShader == NULL ||
        Alpha == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = TranslucentShader->VTable->TranslucentRoutine(TranslucentShader->Data,
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
    _In_opt_ _Post_invalid_ PTRANSLUCENT_SHADER TranslucentShader
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (TranslucentShader == NULL)
    {
        return;
    }

    TranslucentShader->ReferenceCount -= 1;

    if (TranslucentShader->ReferenceCount == 0)
    {
        FreeRoutine = TranslucentShader->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(TranslucentShader->Data);
        }

        IrisAlignedFree(TranslucentShader);
    }
}