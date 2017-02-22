/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_pixelsampler.h

Abstract:

    This file contains the definitions for the PIXEL_SAMPLER type.

--*/

#ifndef _PIXEL_SAMPLER_IRIS_CAMERA_
#define _PIXEL_SAMPLER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPIXEL_SAMPLER_SAMPLE_PIXEL)(
    _In_ PCVOID Context,
    _In_ PCRAY_GENERATOR RayGenerator,
    _In_ PCSAMPLE_RAYTRACER RayTracer,
    _In_ PRANDOM Rng,
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
    );

typedef struct _PIXEL_SAMPLER_VTABLE {
    PPIXEL_SAMPLER_SAMPLE_PIXEL SamplePixelRoutine;
    PFREE_ROUTINE FreeRoutine;
} PIXEL_SAMPLER_VTABLE, *PPIXEL_SAMPLER_VTABLE;

typedef CONST PIXEL_SAMPLER_VTABLE *PCPIXEL_SAMPLER_VTABLE;

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef CONST PIXEL_SAMPLER *PCPIXEL_SAMPLER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
PixelSamplerAllocate(
    _In_ PCPIXEL_SAMPLER_VTABLE PixelSamplerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPIXEL_SAMPLER *PixelSampler
    );

IRISCAMERAAPI
VOID
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER PixelSampler
    );

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_