/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    normal.c

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#include <irisshadingmodel.h>

//
// Types
//

struct _NORMAL {
    PCNORMAL_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PNORMAL
NormalAllocate(
    _In_ PCNORMAL_VTABLE NormalVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PNORMAL Normal;

    if (NormalVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(NORMAL),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    Normal = (PNORMAL) HeaderAllocation;

    Normal->VTable = NormalVTable;
    Normal->Data = DataAllocation;
    Normal->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return Normal;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    if (Normal == NULL ||
        ModelHitPoint == NULL ||
        SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }
    
    Status = Normal->VTable->ComputeNormalRoutine(Normal->Data,
                                                  ModelHitPoint,
                                                  AdditionalData,
                                                  SurfaceNormal);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
NormalPrenormalized(
    _In_ PCNORMAL Normal,
    _Out_ PBOOL Prenormalized
    )
{
    if (Normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    *Prenormalized = Normal->VTable->Prenormalized;

    return ISTATUS_SUCCESS;
}

IRISSHADINGMODELAPI
VOID
NormalReference(
    _In_opt_ PNORMAL Normal
    )
{
    if (Normal == NULL)
    {
        return;
    }

    Normal->ReferenceCount += 1;
}

IRISSHADINGMODELAPI
VOID
NormalDereference(
    _In_opt_ _Post_invalid_ PNORMAL Normal
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Normal == NULL)
    {
        return;
    }

    Normal->ReferenceCount -= 1;

    if (Normal->ReferenceCount == 0)
    {
        FreeRoutine = Normal->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Normal->Data);
        }

        IrisAlignedFree(Normal);
    }
}