/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracer.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ Render function.

--*/

#include <iriscameraplusplus.h>

#ifndef _RENDER_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _RENDER_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Functions
//

static
void
Render(
    _In_ const Camera & CameraRef,
    _In_ const PixelSampler & PixelSamplerRef,
    _In_ const SampleTracerGenerator & SampleTracerGeneratorRef,
    _In_ const RandomGenerator & RandomGeneratorRef,
    _Inout_ Framebuffer & FramebufferRef
    )
{
    ISTATUS Status = IrisCameraRender(CameraRef.AsPCCAMERA(),
                                      PixelSamplerRef.AsPCPIXEL_SAMPLER(),
                                      SampleTracerGeneratorRef.AsPCSAMPLE_TRACER_GENERATOR(),
                                      RandomGeneratorRef.AsPCRANDOM_GENERATOR(),
                                      FramebufferRef.AsPFRAMEBUFFER());

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
}

} // namespace IrisCamera

#endif // _RENDER_IRIS_CAMERA_PLUS_PLUS_HEADER_
