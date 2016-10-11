/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    light.c

Abstract:

    This file contains the definitions for the PBR_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef union _PHYSX_LIGHT_SHARED_REFERENCE_COUNT {
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT Area;
    SIZE_T Simple;
} PHYSX_LIGHT_SHARED_REFERENCE_COUNT;

struct _PBR_LIGHT {
    PCPBR_LIGHT_VTABLE VTable;
    PHYSX_LIGHT_SHARED_REFERENCE_COUNT ReferenceCount;
    PVOID Data;
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightAllocateInternal(
    _In_ PCPBR_LIGHT_VTABLE PbrLightVTable,
    _In_opt_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_LIGHT *PbrLight
    )
{
    ASSERT(PbrLightVTable != NULL);
    ASSERT(ReferenceCount == NULL || PbrLightVTable == &AreaLightAdapterVTable);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(PbrLight != NULL);

    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_LIGHT AllocatedLight;

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PBR_LIGHT),
                                                      _Alignof(PBR_LIGHT),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedLight = (PPBR_LIGHT) HeaderAllocation;

    AllocatedLight->VTable = PbrLightVTable;
    AllocatedLight->Data = DataAllocation;

    if (ReferenceCount != NULL)
    {
        AllocatedLight->ReferenceCount.Area = ReferenceCount;
    }
    else
    {
        AllocatedLight->ReferenceCount.Simple = 1;
    }

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PbrLight = AllocatedLight;

    return ISTATUS_SUCCESS;
}

_Ret_
PVOID
PBRLightGetData(
    _In_ PPBR_LIGHT PBRLight
    )
{
    ASSERT(PBRLight != NULL);
    return PBRLight->Data;
}

VOID
PBRLightFree(
    _In_ _Post_invalid_ PPBR_LIGHT PbrLight
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    ASSERT(PbrLight != NULL);
    
    FreeRoutine = PbrLight->VTable->FreeRoutine;
    
    if (FreeRoutine != NULL)
    {
        FreeRoutine(PbrLight->Data);
    }
    
    IrisAlignedFree(PbrLight);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightAllocate(
    _In_ PCPBR_LIGHT_VTABLE PbrLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_LIGHT *PbrLight
    )
{
    ISTATUS Status;

    if (PbrLightVTable == NULL || 
        PbrLightVTable == &AreaLightAdapterVTable)
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

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = PbrLightAllocateInternal(PbrLightVTable,
                                      NULL,
                                      Data,
                                      DataSizeInBytes,
                                      DataAlignment,
                                      PbrLight);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightSample(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToPbrLight,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PointValidate(HitPoint) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (ToPbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrLight->VTable->SampleRoutine(PbrLight->Data,
                                             HitPoint,
                                             VisibilityTester,
                                             Rng,
                                             Compositor, 
                                             Spectrum,
                                             ToPbrLight,
                                             Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrLightComputeEmissive(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ RAY ToPbrLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToPbrLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = PbrLight->VTable->ComputeEmissiveRoutine(PbrLight->Data,
                                                      ToPbrLight,
                                                      Tester,
                                                      Compositor,
                                                      Spectrum);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightComputeEmissiveWithPdf(
    _In_ PCPBR_LIGHT PbrLight,
    _In_ RAY ToPbrLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToPbrLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = PbrLight->VTable->ComputeEmissiveWithPdfRoutine(PbrLight->Data,
                                                             ToPbrLight,
                                                             Tester,
                                                             Compositor,
                                                             Spectrum,
                                                             Pdf);

    return Status;
}

VOID
PbrLightRetain(
    _In_opt_ PPBR_LIGHT PbrLight
    )
{
    if (PbrLight == NULL)
    {
        return;
    }
    
    if (PbrLight->VTable == &AreaLightAdapterVTable)
    {
        AreaLightReferenceCountRetain(PbrLight->ReferenceCount.Area);
    }
    else
    {
        PbrLight->ReferenceCount.Simple += 1;
    }
}

VOID
PbrLightRelease(
    _In_opt_ _Post_invalid_ PPBR_LIGHT PbrLight
    )
{
    if (PbrLight == NULL)
    {
        return;
    }

    if (PbrLight->VTable == &AreaLightAdapterVTable)
    {
        AreaLightReferenceCountRelease(PbrLight->ReferenceCount.Area);
    }
    else
    {
        PbrLight->ReferenceCount.Simple -= 1;
        
        if (PbrLight->ReferenceCount.Simple == 0)
        {
            PBRLightFree(PbrLight);
        }
    }
}