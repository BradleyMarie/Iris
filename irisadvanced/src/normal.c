/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    normal.c

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#include <irisadvancedp.h>

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
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
NormalAllocate(
    _In_ PCNORMAL_VTABLE NormalVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PNORMAL *Normal
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PNORMAL AllocatedNormal;

    if (NormalVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (Normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(NORMAL),
                                                      _Alignof(NORMAL),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedNormal = (PNORMAL) HeaderAllocation;

    AllocatedNormal->VTable = NormalVTable;
    AllocatedNormal->Data = DataAllocation;
    AllocatedNormal->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Normal = AllocatedNormal;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal, 
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    if (Normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    Status = Normal->VTable->ComputeNormalRoutine(Normal->Data,
                                                  ModelHitPoint,
                                                  AdditionalData,
                                                  SurfaceNormal);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
NormalComputeNormalAndNormalize(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    Status = NormalComputeNormal(Normal,
                                 ModelHitPoint,
                                 AdditionalData,
                                 SurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Normal->VTable->Prenormalized == FALSE)
    {
        *SurfaceNormal = VectorNormalize(*SurfaceNormal, NULL, NULL);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormalAndTransform(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    if (Normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = Normal->VTable->ComputeNormalRoutine(Normal->Data,
                                                  ModelHitPoint,
                                                  AdditionalData,
                                                  SurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *SurfaceNormal = VectorMatrixInverseTransposedMultiply(ModelToWorld,
                                                           *SurfaceNormal);

    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
NormalComputeNormalAndTransformAndNormalize(
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    Status = NormalComputeNormalAndTransform(Normal,
                                             ModelHitPoint,
                                             AdditionalData,
                                             ModelToWorld,
                                             SurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Normal->VTable->Prenormalized == FALSE)
    {
        *SurfaceNormal = VectorNormalize(*SurfaceNormal, NULL, NULL);
    }

    return ISTATUS_SUCCESS;
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
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Prenormalized == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Prenormalized = Normal->VTable->Prenormalized;

    return ISTATUS_SUCCESS;
}

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