/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    emissive.c

Abstract:

    This file contains the definitions for the EMISSIVE_SHADER type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _EMISSIVE_SHADER {
    PCEMISSIVE_SHADER_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_SHADER
EmissiveShaderAllocate(
    _In_ PCEMISSIVE_SHADER_VTABLE EmissiveShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PEMISSIVE_SHADER EmissiveShader;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (EmissiveShaderVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(EMISSIVE_SHADER),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    EmissiveShader = (PEMISSIVE_SHADER) HeaderAllocation;

    EmissiveShader->VTable = EmissiveShaderVTable;
    EmissiveShader->Data = DataAllocation;
    EmissiveShader->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return EmissiveShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
EmissiveShaderShade(
    _In_ PCEMISSIVE_SHADER EmissiveShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    ISTATUS Status;

    if (EmissiveShader == NULL ||
        Emissive == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = EmissiveShader->VTable->EmissiveRoutine(EmissiveShader->Data,
                                                     WorldHitPoint,
                                                     ModelHitPoint,
                                                     AdditionalData,
                                                     Emissive);

    return Status;
}

IRISSHADINGMODELAPI
VOID
EmissiveShaderReference(
    _In_opt_ PEMISSIVE_SHADER EmissiveShader
    )
{   
    if (EmissiveShader == NULL)
    {
        return;
    }

    EmissiveShader->ReferenceCount += 1;
}

IRISSHADINGMODELAPI
VOID
EmissiveShaderDereference(
    _In_opt_ _Post_invalid_ PEMISSIVE_SHADER EmissiveShader
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (EmissiveShader == NULL)
    {
        return;
    }

    EmissiveShader->ReferenceCount -= 1;

    if (EmissiveShader->ReferenceCount == 0)
    {
        FreeRoutine = EmissiveShader->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(EmissiveShader->Data);
        }

        IrisAlignedFree(EmissiveShader);
    }
}