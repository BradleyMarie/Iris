/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracercallback.h

Abstract:

    This file contains the internal definitions for the SAMPLE_TRACER_CALLBACK type.

--*/

#ifndef _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_INTERNAL_
#define _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerCallbackCreateAndTrace(
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCCAMERA Camera,
    _Inout_ PRANDOM Rng,
    _Inout_ PFRAMEBUFFER FrameBuffer
    );

#endif // _SAMPLE_TRACER_CALLBACK_IRIS_CAMERA_INTERNAL_