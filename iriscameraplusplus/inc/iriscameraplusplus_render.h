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
inline
Framebuffer
Render(
    _In_ Camera CameraRef,
    _In_ PixelSampler PixelSamplerRef,
    _In_ SampleTracer SampleTracerRef,
    _Inout_ IrisAdvanced::RandomGenerator RandomGeneratorRef,
    _In_ SIZE_T FramebufferRows,
    _In_ SIZE_T FramebufferColumns
    )
{
    PFRAMEBUFFER Output;

    ISTATUS Status = IrisCameraRender(CameraRef.AsPCCAMERA(),
                                      PixelSamplerRef.AsPCPIXEL_SAMPLER(),
                                      SampleTracerRef.AsPCSAMPLE_TRACER(),
                                      RandomGeneratorRef.AsPCRANDOM_GENERATOR(),
                                      FramebufferRows,
                                      FramebufferColumns,
                                      &Output);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }

    return Framebuffer(Output, false);
}

static
inline
Framebuffer
RenderParallel(
    _In_ Camera CameraRef,
    _In_ PixelSampler PixelSamplerRef,
    _In_ SampleTracer SampleTracerRef,
    _In_ IrisAdvanced::RandomGenerator RandomGeneratorRef,
    _In_ SIZE_T FramebufferRows,
    _In_ SIZE_T FramebufferColumns
    )
{
    PFRAMEBUFFER Output;

    ISTATUS Status = IrisCameraRenderParallel(CameraRef.AsPCCAMERA(),
                                              PixelSamplerRef.AsPCPIXEL_SAMPLER(),
                                              SampleTracerRef.AsPCSAMPLE_TRACER(),
                                              RandomGeneratorRef.AsPCRANDOM_GENERATOR(),
                                              FramebufferRows,
                                              FramebufferColumns,
                                              &Output);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
    
    return Framebuffer(Output, false);
}

} // namespace IrisCamera

#endif // _RENDER_IRIS_CAMERA_PLUS_PLUS_HEADER_
