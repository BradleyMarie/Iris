/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains the definitions for the render function.

--*/

#include <iriscamerap.h>

//
// Public Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IrisCameraRender(
    _In_ PCCAMERA Camera,
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCSAMPLE_TRACER_GENERATOR SampleTracerGenerator,
    _In_ PCRANDOM_GENERATOR RandomGenerator,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    SIZE_T NumberOfPixels;
    SIZE_T PixelIndex;
    ISTATUS Status;

    if (Camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PixelSampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SampleTracerGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (RandomGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    NumberOfPixels = FramebufferRows * FramebufferColumns;

#ifdef _OPENMP
    #pragma omp parallel default(shared) reduction(+: Status) num_threads(NumberOfThreads)
#endif // OPENMP
    {
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

        PRANDOM Rng;
        Status = RandomGeneratorGenerateRandom(RandomGenerator, &Rng);

        if (Status == ISTATUS_SUCCESS)
        {
            PRANDOM_REFERENCE RngReference = RandomGetRandomReference(Rng);

            PSAMPLE_TRACER SampleTracer;
            Status = SampleTracerGeneratorGenerateSampleTracer(SampleTracerGenerator,
                                                               &SampleTracer);

            if (Status == ISTATUS_SUCCESS)
            {
                #pragma omp for schedule(dynamic, 16)
                for (PixelIndex = 0; PixelIndex < NumberOfPixels; PixelIndex++)
                {
                    SIZE_T PixelRow = PixelIndex / FramebufferColumns;
                    SIZE_T PixelColumn = PixelIndex % FramebufferColumns;

                    RAY_GENERATOR RayGenerator = RayGeneratorCreate(Camera,
                                                                    PixelRow,
                                                                    FramebufferRows,
                                                                    PixelColumn,
                                                                    FramebufferColumns);
                    
                    COLOR3 PixelColor;
                    Status = PixelSamplerSamplePixel(PixelSampler,
                                                    &RayGenerator,
                                                    SampleTracer,
                                                    RngReference,
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
                        break;
                    }

                    FramebufferSetPixel(Framebuffer,
                                        PixelColor,
                                        PixelRow,
                                        PixelColumn);
                }

                SampleTracerFree(SampleTracer);
            }
            
            RandomFree(Rng);
        }
    }

    return Status;
}