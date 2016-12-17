/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    pixelsampler.c

Abstract:

    This file contains the definitions for the PIXEL_SAMPLER type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _PIXEL_SAMPLER {
    PCPIXEL_SAMPLER_VTABLE VTable;
    PVOID Data;
};

//
// Private Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PixelSamplerSamplePixel(
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCRAY_GENERATOR RayGenerator,
    _In_ PCCAMERA_RAYTRACER RayTracer,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ BOOL SamplePixel,
    _In_ BOOL SampleLens,
    _In_ FLOAT MinPixelU,
    _In_ FLOAT MaxPixelU,
    _In_ FLOAT MinPixelV,
    _In_ FLOAT MaxPixelV,
    _In_ FLOAT MinLensU,
    _In_ FLOAT MaxLensU,
    _In_ FLOAT MinLensV,
    _In_ FLOAT MaxLensV,
    _Out_ PCOLOR3 Color
    )
{
    ISTATUS Status;

    ASSERT(PixelSampler != NULL);
    ASSERT(RayGenerator != NULL);
    ASSERT(RayTracer != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Color != NULL);

    Status = PixelSampler->VTable->SamplePixelRoutine(PixelSampler->Data,
                                                      RayGenerator,
                                                      RayTracer,
                                                      Rng,
                                                      SamplePixel,
                                                      SampleLens,
                                                      MinPixelU,
                                                      MaxPixelU,
                                                      MinPixelV,
                                                      MaxPixelV,
                                                      MinLensU,
                                                      MaxLensU,
                                                      MinLensV,
                                                      MaxLensV,
                                                      Color);
    
    return Status;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PixelSamplerAllocate(
    _In_ PCPIXEL_SAMPLER_VTABLE PixelSamplerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPIXEL_SAMPLER *PixelSampler
    )
{
    PPIXEL_SAMPLER AllocatedPixelSampler;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (PixelSamplerVTable == NULL)
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

    if (PixelSampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PIXEL_SAMPLER),
                                                      _Alignof(PIXEL_SAMPLER),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedPixelSampler = (PPIXEL_SAMPLER) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedPixelSampler->VTable = PixelSamplerVTable;
    AllocatedPixelSampler->Data = DataAllocation;

    *PixelSampler = AllocatedPixelSampler;

    return ISTATUS_SUCCESS;
}

VOID
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER PixelSampler
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (PixelSampler == NULL)
    {
        return;
    }
    
    FreeRoutine = PixelSampler->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(PixelSampler->Data);
    }

    IrisAlignedFree(PixelSampler);
}
