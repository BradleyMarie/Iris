/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracerthreadstatecallback.c

Abstract:

    This file contains the definitions for the SAMPLE_TRACER_CALLBACK type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _SAMPLE_TRACER_CALLBACK {
    PCPIXEL_SAMPLER PixelSampler;
    PCSAMPLE_TRACER SampleTracer;
    PCCAMERA Camera;
    PRANDOM Rng;
    PFRAMEBUFFER Framebuffer;
};

//
// Static Functions
//

STATIC
SAMPLE_TRACER_CALLBACK
SampleTracerCallbackCreate(
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCCAMERA Camera,
    _Inout_ PRANDOM Rng,
    _Inout_ PFRAMEBUFFER FrameBuffer
    )
{
    SAMPLE_TRACER_CALLBACK Result;

    ASSERT(PixelSampler != NULL);
    ASSERT(SampleTracer != NULL);
    ASSERT(Camera != NULL);
    ASSERT(Rng != NULL);
    ASSERT(FrameBuffer != NULL);

    Result.PixelSampler = PixelSampler;
    Result.SampleTracer = SampleTracer;
    Result.Camera = Camera;
    Result.Rng = Rng;
    Result.Framebuffer = FrameBuffer;

    return Result;
}

//
// Private Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerCallbackCreateAndTrace(
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCCAMERA Camera,
    _Inout_ PRANDOM Rng,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    SAMPLE_TRACER_CALLBACK ThreadStateCallback;
    ISTATUS Status;

    ASSERT(PixelSampler != NULL);
    ASSERT(SampleTracer != NULL);
    ASSERT(Camera != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Framebuffer != NULL);

    ThreadStateCallback = SampleTracerCallbackCreate(PixelSampler,
                                                     SampleTracer,
                                                     Camera,
                                                     Rng,
                                                     Framebuffer);

    Status = SampleTracerGenerateThreadStateAndCallback(SampleTracer,
                                                        &ThreadStateCallback);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    return ISTATUS_SUCCESS;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerCallback(
    _In_ PCSAMPLE_TRACER_CALLBACK Callback,
    _In_opt_ PVOID ThreadState
    )
{
    ISTATUS Status;

    if (Callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = IrisRenderCallback(ThreadState,
                                Callback->PixelSampler,
                                Callback->SampleTracer,
                                Callback->Camera,
                                Callback->Rng,
                                Callback->Framebuffer);

    return Status;
}