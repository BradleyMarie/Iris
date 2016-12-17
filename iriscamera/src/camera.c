/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    camera.c

Abstract:

    This file contains the definitions for the CAMERA type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _CAMERA {
    PCCAMERA_VTABLE VTable;
    PVOID Data;
};

//
// Private Functions
//

VOID
CameraGetParameters(
    _In_ PCCAMERA Camera,
    _Out_ PBOOL SamplePixel,
    _Out_ PBOOL SampleLens,
    _Out_ PFLOAT MinPixelU,
    _Out_ PFLOAT MaxPixelU,
    _Out_ PFLOAT MinPixelV,
    _Out_ PFLOAT MaxPixelV,
    _Out_ PFLOAT MinLensU,
    _Out_ PFLOAT MaxLensU,
    _Out_ PFLOAT MinLensV,
    _Out_ PFLOAT MaxLensV
    )
{
    ASSERT(Camera != NULL);
    ASSERT(SamplePixel != NULL);
    ASSERT(SampleLens != NULL);
    ASSERT(MinPixelU != NULL);
    ASSERT(MaxPixelU != NULL);
    ASSERT(MinPixelV != NULL);
    ASSERT(MaxPixelV != NULL);
    ASSERT(MinLensU != NULL);
    ASSERT(MaxLensU != NULL);
    ASSERT(MinLensV != NULL);
    ASSERT(MaxLensV != NULL);

    Camera->VTable->GetParamtersRoutine(Camera->Data,
                                        SamplePixel,
                                        SampleLens,
                                        MinPixelU,
                                        MaxPixelU,
                                        MinPixelV,
                                        MaxPixelV,
                                        MinLensU,
                                        MaxLensU,
                                        MinLensV,
                                        MaxLensV);
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraComputeRay(
    _In_ PCCAMERA Camera,
    _In_range_(0, NumberOfRows) SIZE_T Row,
    _In_ SIZE_T NumberOfRows,
    _In_range_(0, NumberOfColumns) SIZE_T Column,
    _In_ SIZE_T NumberOfColumns,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    )
{
    ISTATUS Status;

    ASSERT(Camera != NULL);
    ASSERT(Row < NumberOfRows);
    ASSERT(Column < NumberOfColumns);
    ASSERT(WorldRay != NULL);

    Status = Camera->VTable->ComputeRayRoutine(Camera->Data,
                                               Row,
                                               NumberOfRows,
                                               Column,
                                               NumberOfColumns,
                                               PixelU,
                                               PixelV,
                                               LensU,
                                               LensV,
                                               WorldRay);
    
    return Status;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraAllocate(
    _In_ PCCAMERA_VTABLE CameraVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCAMERA *Camera
    )
{
    PCAMERA AllocatedCamera;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (CameraVTable == NULL)
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

    if (Camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(CAMERA),
                                                      _Alignof(CAMERA),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedCamera = (PCAMERA) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedCamera->VTable = CameraVTable;
    AllocatedCamera->Data = DataAllocation;

    *Camera = AllocatedCamera;

    return ISTATUS_SUCCESS;
}

VOID
CameraFree(
    _In_opt_ _Post_invalid_ PCAMERA Camera
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Camera == NULL)
    {
        return;
    }
    
    FreeRoutine = Camera->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(Camera->Data);
    }

    IrisAlignedFree(Camera);
}
