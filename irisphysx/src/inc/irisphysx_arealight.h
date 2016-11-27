/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_arealight.h

Abstract:

    This file contains the internal definitions for the PHYSX_AREA_LIGHT type;

--*/

#ifndef _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_
#define _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_AREA_LIGHT {
    PCPHYSX_AREA_LIGHT_VTABLE VTable;
    PVOID Data;
} PHYSX_AREA_LIGHT, *PPHYSX_AREA_LIGHT;

typedef CONST PHYSX_AREA_LIGHT *PCPHYSX_AREA_LIGHT; 

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxAreaLightAllocate(
    _In_ PCPHYSX_AREA_LIGHT_VTABLE AreaLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_AREA_LIGHT *AreaLight
    )
{
    PPHYSX_AREA_LIGHT AllocatedAreaLight;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    ASSERT(AreaLightVTable != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(AreaLight != NULL);

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_AREA_LIGHT),
                                                      _Alignof(PHYSX_AREA_LIGHT),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedAreaLight = (PPHYSX_AREA_LIGHT) HeaderAllocation;

    AllocatedAreaLight->VTable = AreaLightVTable;
    AllocatedAreaLight->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *AreaLight = AllocatedAreaLight;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
ISTATUS
PhysxAreaLightComputeEmissive(
    _In_ PCPHYSX_AREA_LIGHT AreaLight,
    _In_ POINT3 OnLight,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    ASSERT(AreaLight != NULL);
    ASSERT(PointValidate(OnLight) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Spectrum != NULL);

    Status = AreaLight->VTable->ComputeEmissiveRoutine(AreaLight->Data,
                                                       OnLight,
                                                       Compositor,
                                                       Spectrum);
    
    return Status;
}

SFORCEINLINE
VOID
PhysxAreaLightFree(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT AreaLight
    )
{
    if (AreaLight == NULL)
    {
        return;
    }

    AreaLight->VTable->FreeRoutine(AreaLight->Data);
    IrisAlignedFree(AreaLight);
}

#endif // _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_
