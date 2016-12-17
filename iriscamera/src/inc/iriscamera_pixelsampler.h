/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_pixelsampler.h

Abstract:

    This file contains the internal definitions for the PIXEL_SAMPLER type.

--*/

#ifndef _PIXEL_SAMPLER_IRIS_CAMERA_INTERNAL_
#define _PIXEL_SAMPLER_IRIS_CAMERA_INTERNAL_

#include <iriscamera.h>

//
// Functions
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
    );

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_INTERNAL_