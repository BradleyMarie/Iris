/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    light.c

Abstract:

    This file contains the definitions for the PHYSX_LIGHT type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef union _PHYSX_LIGHT_SHARED_REFERENCE_COUNT {
    PPHYSX_AREA_LIGHT_REFERENCE_COUNT Area;
    SIZE_T Simple;
} PHYSX_LIGHT_SHARED_REFERENCE_COUNT;

struct _PHYSX_LIGHT {
    PCPHYSX_LIGHT_VTABLE VTable;
    PHYSX_LIGHT_SHARED_REFERENCE_COUNT ReferenceCount;
    PVOID Data;
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightAllocateInternal(
    _In_ PCPHYSX_LIGHT_VTABLE LightVTable,
    _In_opt_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_LIGHT *Light
    )
{
    ASSERT(LightVTable != NULL);
    ASSERT(ReferenceCount == NULL || LightVTable == &AreaLightAdapterVTable);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(Light != NULL);

    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPHYSX_LIGHT AllocatedLight;

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_LIGHT),
                                                      _Alignof(PHYSX_LIGHT),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedLight = (PPHYSX_LIGHT) HeaderAllocation;

    AllocatedLight->VTable = LightVTable;
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

    *Light = AllocatedLight;

    return ISTATUS_SUCCESS;
}

_Ret_
PVOID
PhysxLightGetData(
    _In_ PPHYSX_LIGHT Light
    )
{
    ASSERT(Light != NULL);
    return Light->Data;
}

VOID
PhysxLightFree(
    _In_ _Post_invalid_ PPHYSX_LIGHT Light
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    ASSERT(Light != NULL);
    
    FreeRoutine = Light->VTable->FreeRoutine;
    
    if (FreeRoutine != NULL)
    {
        FreeRoutine(Light->Data);
    }
    
    IrisAlignedFree(Light);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightAllocate(
    _In_ PCPHYSX_LIGHT_VTABLE LightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_LIGHT *Light
    )
{
    ISTATUS Status;

    if (LightVTable == NULL ||
        LightVTable == &AreaLightAdapterVTable)
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

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = PhysxLightAllocateInternal(LightVTable,
                                        NULL,
                                        Data,
                                        DataSizeInBytes,
                                        DataAlignment,
                                        Light);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightSample(
    _In_ PCPHYSX_LIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Light == NULL)
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

    if (ToLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = Light->VTable->SampleRoutine(Light->Data,
                                          HitPoint,
                                          VisibilityTester,
                                          Rng,
                                          Compositor,
                                          Spectrum,
                                          ToLight,
                                          Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightComputeEmissive(
    _In_ PCPHYSX_LIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VisibilityTester == NULL)
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

    Status = Light->VTable->ComputeEmissiveRoutine(Light->Data,
                                                   ToLight,
                                                   VisibilityTester,
                                                   Compositor,
                                                   Spectrum);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxLightComputeEmissiveWithPdf(
    _In_ PCPHYSX_LIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(ToLight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VisibilityTester == NULL)
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

    Status = Light->VTable->ComputeEmissiveWithPdfRoutine(Light->Data,
                                                          ToLight,
                                                          VisibilityTester,
                                                          Compositor,
                                                          Spectrum,
                                                          Pdf);

    return Status;
}

VOID
PhysxLightRetain(
    _In_opt_ PPHYSX_LIGHT Light
    )
{
    if (Light == NULL)
    {
        return;
    }
    
    if (Light->VTable == &AreaLightAdapterVTable)
    {
        AreaLightReferenceCountRetain(Light->ReferenceCount.Area);
    }
    else
    {
        Light->ReferenceCount.Simple += 1;
    }
}

VOID
PhysxLightRelease(
    _In_opt_ _Post_invalid_ PPHYSX_LIGHT Light
    )
{
    if (Light == NULL)
    {
        return;
    }

    if (Light->VTable == &AreaLightAdapterVTable)
    {
        AreaLightReferenceCountRelease(Light->ReferenceCount.Area);
    }
    else
    {
        Light->ReferenceCount.Simple -= 1;
        
        if (Light->ReferenceCount.Simple == 0)
        {
            PhysxLightFree(Light);
        }
    }
}
