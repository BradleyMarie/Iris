/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    indirect.c

Abstract:

    This file contains the definitions for the INDIRECT_MATERIAL type.

--*/

#include <irisrgbp.h>

//
// Types
//

struct _INDIRECT_SHADER {
    PCINDIRECT_SHADER_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
IndirectShaderAllocate(
    _In_ PCINDIRECT_SHADER_VTABLE IndirectShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PINDIRECT_SHADER IndirectShader;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (IndirectShaderVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(INDIRECT_SHADER),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    IndirectShader = (PINDIRECT_SHADER) HeaderAllocation;

    IndirectShader->VTable = IndirectShaderVTable;
    IndirectShader->Data = DataAllocation;
    IndirectShader->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return IndirectShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IndirectShaderShade(
    _In_ PCINDIRECT_SHADER IndirectShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    )
{
    ISTATUS Status;

    if (IndirectShader == NULL ||
        SurfaceNormal == NULL ||
        Rng == NULL ||
        VisibilityTester == NULL ||
        Indirect == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    Status = IndirectShader->VTable->IndirectRoutine(IndirectShader->Data,
                                                     WorldHitPoint,
                                                     ModelHitPoint,
                                                     WorldViewer,
                                                     ModelViewer,
                                                     AdditionalData,
                                                     SurfaceNormal,
                                                     Rng,
                                                     VisibilityTester,
                                                     RayTracer,
                                                     Indirect);

    return Status;
}

VOID
IndirectShaderReference(
    _In_opt_ PINDIRECT_SHADER IndirectShader
    )
{
    if (IndirectShader == NULL)
    {
        return;
    }

    IndirectShader->ReferenceCount += 1;
}

VOID
IndirectShaderDereference(
    _In_opt_ _Post_invalid_ PINDIRECT_SHADER IndirectShader
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (IndirectShader == NULL)
    {
        return;
    }

    IndirectShader->ReferenceCount -= 1;

    if (IndirectShader->ReferenceCount == 0)
    {
        FreeRoutine = IndirectShader->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(IndirectShader->Data);
        }

        IrisAlignedFree(IndirectShader);
    }
}