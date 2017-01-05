/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_render.h

Abstract:

    This file contains the definitions for the render functions.

--*/

#ifndef _RENDER_IRIS_CAMERA_
#define _RENDER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
IrisCameraRender(
    _In_ PCCAMERA Camera,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER_GENERATOR SampleTracerGenerator,
    _In_ PCRANDOM_GENERATOR RngGenerator,
    _In_ SIZE_T FrameBufferRows,
    _In_ SIZE_T FrameBufferColumns,
    _Out_ PFRAMEBUFFER *FrameBuffer
    );

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
IrisCameraRenderParallel(
    _In_ PCCAMERA Camera,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER_GENERATOR SampleTracerGenerator,
    _In_ PCRANDOM_GENERATOR RandomGenerator,
    _In_ SIZE_T FrameBufferRows,
    _In_ SIZE_T FrameBufferColumns,
    _Out_ PFRAMEBUFFER *FrameBuffer
    );

#endif // _RENDER_IRIS_CAMERA_