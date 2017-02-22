/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracergenerator.h

Abstract:

    This file contains the internal definitions for the SAMPLE_TRACER_GENERATOR type.

--*/

#ifndef _RENDER_IRIS_CAMERA_INTERNAL_
#define _RENDER_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IrisRenderCallback(
    _In_opt_ PCVOID ThreadState,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCCAMERA Camera,
    _In_ PRANDOM Rng,
    _In_ PFRAMEBUFFER Framebuffer
    );

#endif // _RENDER_IRIS_CAMERA_INTERNAL_