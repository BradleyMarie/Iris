/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the CAMERA_RAYTRACER type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _CAMERA_RAYTRACER {
    PCCAMERA_RAYTRACER_VTABLE VTable;
    PVOID Data;
};

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraRayTracerAllocate(
    _In_ PCCAMERA_RAYTRACER_VTABLE CameraRayTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCAMERA_RAYTRACER *CameraRayTracer
    )
{
    PCAMERA_RAYTRACER AllocatedCameraRayTracer;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (CameraRayTracerVTable == NULL)
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

    if (CameraRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(CAMERA_RAYTRACER),
                                                      _Alignof(CAMERA_RAYTRACER),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedCameraRayTracer = (PCAMERA_RAYTRACER) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedCameraRayTracer->VTable = CameraRayTracerVTable;
    AllocatedCameraRayTracer->Data = DataAllocation;

    *CameraRayTracer = AllocatedCameraRayTracer;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraRayTracerTraceRay(
    _In_ PCCAMERA_RAYTRACER CameraRayTracer,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCOLOR3 Color
    )
{
    ISTATUS Status;

    if (CameraRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = CameraRayTracer->VTable->TraceRayRoutine(CameraRayTracer->Data,
                                                      WorldRay,
                                                      Rng,
                                                      Color);
    
    return Status;
}

VOID
CameraRayTracerFree(
    _In_opt_ _Post_invalid_ PCAMERA_RAYTRACER CameraRayTracer
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (CameraRayTracer == NULL)
    {
        return;
    }
    
    FreeRoutine = CameraRayTracer->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(CameraRayTracer->Data);
    }

    IrisAlignedFree(CameraRayTracer);
}
