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
    _In_opt_ PCPIXEL_SAMPLER PixelSampler,
    _In_ PCCAMERA_RAYTRACER CameraRayTracer,
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

    if (CameraRayTracer == NULL)
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
        Status = RandomGeneratorAllocateRandom(RandomGenerator, &Rng);

        if (Status == ISTATUS_SUCCESS)
        {
            PRANDOM_REFERENCE RngReference = RandomGetRandomReference(Rng);

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
                if (PixelSampler != NULL)
                {
                    Status = PixelSamplerSamplePixel(PixelSampler,
                                                    &RayGenerator,
                                                    CameraRayTracer,
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
                }
                else
                {
                    FLOAT PixelU = (FLOAT) 0.5f * (MaxPixelU - MinPixelU);
                    FLOAT PixelV = (FLOAT) 0.5f * (MaxPixelU - MinPixelU);
                    FLOAT LensU = (FLOAT) 0.5f * (MaxLensU - MinLensU);
                    FLOAT LensV = (FLOAT) 0.5f * (MaxLensV - MinLensV);

                    RAY WorldRay;
                    RayGeneratorGenerateRay(&RayGenerator,
                                            PixelU,
                                            PixelV,
                                            LensU,
                                            LensV,
                                            &WorldRay);

                    Status = CameraRayTracerTraceRay(CameraRayTracer,
                                                     WorldRay,
                                                     RngReference,
                                                     &PixelColor);
                }

                if (Status != ISTATUS_SUCCESS)
                {
                    break;
                }

                FramebufferSetPixel(Framebuffer,
                                    PixelColor,
                                    PixelRow,
                                    PixelColumn);
            }
            
            RandomFree(Rng);
        }
    }

    return Status;
}