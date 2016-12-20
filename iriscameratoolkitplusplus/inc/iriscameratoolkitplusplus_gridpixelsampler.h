/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkitplusplus_gridpixelsampler.h

Abstract:

    This file contains the definition for the GridPixelSampler type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
#define _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_

namespace IrisCameraToolkit {
namespace GridPixelSampler {

//
// Functions
//

static
inline
IrisCamera::PixelSampler
Create(
    _In_ UINT16 XSamplesPerPixel,
    _In_ UINT16 YSamplesPerPixel,
    _In_ bool Jitter
    )
{
    PPIXEL_SAMPLER PixelSamplerPtr;
    ISTATUS Status = GridPixelSamplerAllocate(XSamplesPerPixel,
                                              YSamplesPerPixel,
                                              (Jitter) ? TRUE : FALSE,
                                              &PixelSamplerPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("XSamplesPerPixel");
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("YSamplesPerPixel");
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisCamera::PixelSampler(PixelSamplerPtr);
}

} // namespace GridPixelSampler
} // namespace IrisCameraToolkit

#endif // _GRID_PIXEL_SAMPLER_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
