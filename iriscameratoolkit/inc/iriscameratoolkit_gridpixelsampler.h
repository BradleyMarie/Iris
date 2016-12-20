/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkit_gridpixelsampler.h

Abstract:

    This file contains the definition of the GRID_PIXEL_SAMPLER type.

--*/

#ifndef _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_
#define _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_

#include <iriscameratoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERATOOLKITAPI
ISTATUS
GridPixelSamplerAllocate(
    _In_ UINT16 XSamplesPerPixel,
    _In_ UINT16 YSamplesPerPixel,
    _In_ BOOL Jitter,
    _Out_ PPIXEL_SAMPLER *PixelSampler
    );

#endif // _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_