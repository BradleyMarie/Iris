/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains the definitions for the render function.

--*/

#include <iriscamerap.h>

//
// Types
//

typedef struct _CALLBACK_CONTEXT {
    PCPIXEL_SAMPLER PixelSampler;
    PCSAMPLE_TRACER SampleTracer;
    PCCAMERA Camera;
    PFRAMEBUFFER Framebuffer;
    ISTATUS Status;
} CALLBACK_CONTEXT, *PCALLBACK_CONTEXT;

//
// Private Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IrisRenderCallback(
    _In_opt_ PVOID ThreadState,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCCAMERA Camera,
    _In_ PRANDOM Rng,
    _In_ PFRAMEBUFFER Framebuffer
    )
{
    SAMPLE_RAYTRACER SampleRayTracer;
    RAY_GENERATOR RayGenerator;
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    SIZE_T PixelColumn;
    SIZE_T PixelRow;
    COLOR3 PixelColor;
    BOOL SamplePixel;
    BOOL SampleLens;
    FLOAT MinPixelU;
    FLOAT MaxPixelU;
    FLOAT MinPixelV;
    FLOAT MaxPixelV;
    FLOAT MinLensU;
    FLOAT MaxLensU;
    FLOAT MinLensV;
    FLOAT MaxLensV;
    ISTATUS Status;

#ifndef _MSC_VER
    SIZE_T NumberOfPixels;
    SIZE_T PixelIndex;
#else
    INT64 NumberOfPixels;
    INT64 PixelIndex;
#endif

    ASSERT(PixelSampler != NULL);
    ASSERT(SampleTracer != NULL);
    ASSERT(Camera != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Framebuffer != NULL);

    FramebufferGetDimensions(Framebuffer,
                             &FramebufferRows,
                             &FramebufferColumns);

    NumberOfPixels = FramebufferRows * FramebufferColumns;

    CameraGetParameters(Camera,
                        &SamplePixel,
                        &SampleLens,
                        &MinPixelU,
                        &MaxPixelU,
                        &MinPixelV,
                        &MaxPixelV,
                        &MinLensU,
                        &MaxLensU,
                        &MinLensV,
                        &MaxLensV);

    SampleRayTracer = SamplerRayTracerCreate(SampleTracer, ThreadState, Rng);

#ifdef _OPENMP
    #pragma omp for schedule(dynamic, 16)
#endif // OPENMP
    for (PixelIndex = 0; PixelIndex < NumberOfPixels; PixelIndex += 1)
    {
        PixelRow = (SIZE_T) PixelIndex / FramebufferColumns;
        PixelColumn = (SIZE_T) PixelIndex % FramebufferColumns;

        RayGenerator = RayGeneratorCreate(Camera,
                                          PixelRow,
                                          FramebufferRows,
                                          PixelColumn,
                                          FramebufferColumns);

        Status = PixelSamplerSamplePixel(PixelSampler,
                                         &RayGenerator,
                                         &SampleRayTracer,
                                         Rng,
                                         SamplePixel,
                                         SampleLens,
                                         MinPixelU,
                                         MaxPixelU,
                                         MinPixelV,
                                         MaxPixelV,
                                         MinLensU,
                                         MaxLensU,
                                         MinLensV,
                                         MaxLensV,
                                         &PixelColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        FramebufferSetPixel(Framebuffer,
                            PixelColor,
                            PixelRow,
                            PixelColumn);
    }

    return ISTATUS_SUCCESS;
}


//
// Static Functions
//

STATIC
VOID
RenderWithRandomCallback(
    _Inout_opt_ PVOID Context,
    _In_ PRANDOM Rng
    )
{
    PCALLBACK_CONTEXT CallbackContext;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Rng != NULL);

    CallbackContext = (PCALLBACK_CONTEXT) Context;

    Status = SampleTracerCallbackCreateAndTrace(CallbackContext->PixelSampler,
                                                CallbackContext->SampleTracer,
                                                CallbackContext->Camera,
                                                Rng,
                                                CallbackContext->Framebuffer);

    CallbackContext->Status = Status;
}

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
IrisCameraRenderInternal(
    _In_ PCCAMERA Camera,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCRANDOM_GENERATOR RandomGenerator,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    CALLBACK_CONTEXT CallbackContext;
    ISTATUS Status;

    ASSERT(Camera != NULL);
    ASSERT(PixelSampler != NULL);
    ASSERT(RandomGenerator != NULL);
    ASSERT(Framebuffer != NULL);

    CallbackContext.Camera = Camera;
    CallbackContext.SampleTracer = SampleTracer;
    CallbackContext.PixelSampler = PixelSampler;
    CallbackContext.Framebuffer = Framebuffer;

    Status = RandomGeneratorGenerate(RandomGenerator,
                                     RenderWithRandomCallback,
                                     &CallbackContext);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    return CallbackContext.Status;
}

//
// Public Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IrisCameraRender(
    _In_ PCCAMERA Camera,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCRANDOM_GENERATOR RandomGenerator,
    _In_ SIZE_T FramebufferRows,
    _In_ SIZE_T FramebufferColumns,
    _Out_ PFRAMEBUFFER *Framebuffer
    )
{
    PFRAMEBUFFER AllocatedFramebuffer;
    ISTATUS Status;

    if (Camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PixelSampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SampleTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (RandomGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (FramebufferRows == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (FramebufferColumns == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = FramebufferAllocate(FramebufferRows,
                                 FramebufferColumns,
                                 &AllocatedFramebuffer);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    Status = IrisCameraRenderInternal(Camera,
                                      SampleTracer,
                                      PixelSampler,
                                      RandomGenerator,
                                      AllocatedFramebuffer);

    if (Status != ISTATUS_SUCCESS)
    {
        FramebufferRelease(AllocatedFramebuffer);
        return Status;
    }

    *Framebuffer = AllocatedFramebuffer;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IrisCameraRenderParallel(
    _In_ PCCAMERA Camera,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PCRANDOM_GENERATOR RandomGenerator,
    _In_ SIZE_T FramebufferRows,
    _In_ SIZE_T FramebufferColumns,
    _Out_ PFRAMEBUFFER *Framebuffer
    )
{
    PFRAMEBUFFER AllocatedFramebuffer;
    ISTATUS Status;

    if (Camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PixelSampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SampleTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (RandomGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (FramebufferRows == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (FramebufferColumns == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = FramebufferAllocate(FramebufferRows,
                                 FramebufferColumns,
                                 &AllocatedFramebuffer);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }
    
    Status = ISTATUS_SUCCESS;

#ifdef _OPENMP
    #pragma omp parallel default(shared) reduction(+: Status)
#endif // OPENMP
    {
        Status = IrisCameraRenderInternal(Camera,
                                          SampleTracer,
                                          PixelSampler,
                                          RandomGenerator,
                                          AllocatedFramebuffer);
    }

    if (Status != ISTATUS_SUCCESS)
    {
        FramebufferRelease(AllocatedFramebuffer);
        return Status;
    }

    *Framebuffer = AllocatedFramebuffer;

    return ISTATUS_SUCCESS;
}