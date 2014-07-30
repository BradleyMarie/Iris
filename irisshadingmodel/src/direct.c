/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    direct.c

Abstract:

    This file contains the definitions for the DIRECT_MATERIAL type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _DIRECT_SHADER {
    PCDIRECT_SHADER_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PDIRECT_SHADER
DirectShaderAllocate(
    _In_ PCDIRECT_SHADER_VTABLE DirectShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PDIRECT_SHADER DirectShader;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (DirectShaderVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(DIRECT_SHADER),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    DirectShader = (PDIRECT_SHADER) HeaderAllocation;

    DirectShader->VTable = DirectShaderVTable;
    DirectShader->Data = DataAllocation;
    DirectShader->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return DirectShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
DirectShaderShade(
    _In_ PCDIRECT_SHADER DirectShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    ISTATUS Status;

    if (DirectShader == NULL ||
        SurfaceNormal == NULL ||
        Rng == NULL ||
        VisibilityTester == NULL ||
        Direct == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = DirectShader->VTable->DirectRoutine(DirectShader->Data,
                                                 WorldHitPoint,
                                                 ModelHitPoint,
                                                 WorldViewer,
                                                 ModelViewer,
                                                 AdditionalData,
                                                 SurfaceNormal,
                                                 Rng,
                                                 VisibilityTester,
                                                 Direct);

    return Status;
}

VOID
DirectShaderReference(
    _In_opt_ PDIRECT_SHADER DirectShader
    )
{
    if (DirectShader == NULL)
    {
        return;
    }

    DirectShader->ReferenceCount += 1;
}

VOID
DirectShaderDereference(
    _In_opt_ _Post_invalid_ PDIRECT_SHADER DirectShader
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (DirectShader == NULL)
    {
        return;
    }

    DirectShader->ReferenceCount -= 1;

    if (DirectShader->ReferenceCount == 0)
    {
        FreeRoutine = DirectShader->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(DirectShader->Data);
        }

        IrisAlignedFree(DirectShader);
    }
}