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
    : Data(PixelSamplerPtr)
    { 
        if (PixelSamplerPtr == nullptr)
        {
            throw std::invalid_argument("PixelSamplerPtr");
        }
    }

    PixelSampler(
        _In_ PixelSampler && ToMove
        )
    : Data(ToMove.Data)
    { 
        ToMove.Data = nullptr;
    }

    _Ret_
    PCPIXEL_SAMPLER
    AsPCPIXEL_SAMPLER(
        void
        ) const
    {
        return Data;
    }

    PixelSampler(
        _In_ const PixelSampler & ToCopy
        ) = delete;
        
    PixelSampler &
    operator=(
        _In_ const PixelSampler & ToCopy
        ) = delete;

    ~PixelSampler(
        void
        )
    {
        PixelSamplerFree(Data);
    }

private:
    PPIXEL_SAMPLER Data;
};

} // namespace IrisCamera

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_PLUS_PLUS_HEADER_
