/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_pixelsampler.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ PixelSampler type.

--*/

#include <iriscameraplusplus.h>

#ifndef _PIXEL_SAMPLER_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _PIXEL_SAMPLER_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class PixelSampler final {
public:
    PixelSampler(
        _In_ PPIXEL_SAMPLER PixelSamplerPtr
        )
    : Data(PixelSamplerPtr, [](PPIXEL_SAMPLER ToFree){ PixelSamplerFree(ToFree); })
    { 
        if (PixelSamplerPtr == nullptr)
        {
            throw std::invalid_argument("PixelSamplerPtr");
        }
    }

    _Ret_
    PCPIXEL_SAMPLER
    AsPCPIXEL_SAMPLER(
        void
        ) const
    {
        return Data.get();
    }

private:
    std::shared_ptr<PIXEL_SAMPLER> Data;
};

} // namespace IrisCamera

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_PLUS_PLUS_HEADER_
