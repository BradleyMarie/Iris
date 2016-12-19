/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    gridpixelsampler.c

Abstract:

    This file contains the definitions for the GRID_PIXEL_SAMPLER type.

--*/

#include <iriscameratoolkit.h>

//
// Types
//

typedef struct _GRID_PIXEL_SAMPLER {
    UINT32 XSamplesPerPixel;
    UINT32 YSamplesPerPixel;
    FLOAT SampleWeight;
    BOOL Jitter;
} GRID_PIXEL_SAMPLER, *PGRID_PIXEL_SAMPLER;

typedef CONST GRID_PIXEL_SAMPLER *PCGRID_PIXEL_SAMPLER;

//
// Static functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
GridPixelSamplerSamplePixel(
    _In_ PCVOID Context,
    _In_ PCRAY_GENERATOR RayGenerator,
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ BOOL SamplePixel,
    _In_ BOOL SampleLens,
    _In_ FLOAT MinPixelU,
    _In_ FLOAT MaxPixelU,
    _In_ FLOAT MinPixelV,
    _In_ FLOAT MaxPixelV,
    _In_ FLOAT MinLensU,
    _In_ FLOAT MaxLensU,
    _In_ FLOAT MinLensV,
    _In_ FLOAT MaxLensV,
    _Out_ PCOLOR3 Color
    )
{
    PCGRID_PIXEL_SAMPLER GridPixelSampler;
    FLOAT PixelUIncrement;
    FLOAT PixelVIncrement;
    FLOAT LensUIncrement;
    FLOAT LensVIncrement;
    UINT32 XSampleIndex;
    UINT32 YSampleIndex;
    COLOR3 SampleColor;
    COLOR3 PixelColor;
    FLOAT PixelUBase;
    FLOAT PixelVBase;
    FLOAT LensUBase;
    FLOAT LensVBase;
    FLOAT PixelU;
    FLOAT PixelV;
    FLOAT LensU;
    FLOAT LensV;
    RAY WorldRay;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayGenerator != NULL);
    ASSERT(SampleTracer != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Color != NULL);

    GridPixelSampler = (PCGRID_PIXEL_SAMPLER) Context;

    PixelColor = Color3InitializeBlack();

    if (SamplePixel != FALSE)
    {
        PixelUIncrement = (MaxPixelU - MinPixelU) / (FLOAT) GridPixelSampler->XSamplesPerPixel;
        PixelVIncrement = (MaxPixelV - MinPixelV) / (FLOAT) GridPixelSampler->YSamplesPerPixel;
    }
    else
    {
        PixelUIncrement = (FLOAT) 0.0f;
        PixelVIncrement = (FLOAT) 0.0f;

        MinPixelU = (MaxPixelU - MinPixelU) * (FLOAT) 0.5f;
        MaxPixelU = MinPixelU;

        MinPixelV = (MaxPixelV - MinPixelV) * (FLOAT) 0.5f;
        MaxPixelV = MinPixelV;
    }

    if (SampleLens != FALSE)
    {
        LensUIncrement = (MaxLensU - MinLensU) / (FLOAT) GridPixelSampler->XSamplesPerPixel;
        LensVIncrement = (MaxLensV - MinLensV) / (FLOAT) GridPixelSampler->YSamplesPerPixel;
    }
    else
    {
        LensUIncrement = (FLOAT) 0.0f;
        LensVIncrement = (FLOAT) 0.0f;

        MinLensU = (MaxLensU - MinLensU) * (FLOAT) 0.5f;
        MaxLensU = MinLensU;

        MinLensV = (MaxLensV - MinLensV) * (FLOAT) 0.5f;
        MaxLensV = MinLensV;
    }

    PixelUBase = MinPixelU;
    LensUBase = MinLensU;

    for (XSampleIndex = 0; 
         XSampleIndex < GridPixelSampler->XSamplesPerPixel;
         XSampleIndex += 1)
    {
        PixelVBase = MinPixelV;
        LensVBase = MinLensV;

        for (YSampleIndex = 0; 
             YSampleIndex < GridPixelSampler->YSamplesPerPixel;
             YSampleIndex += 1)
        {
            if (GridPixelSampler->Jitter != FALSE)
            {
                RandomReferenceGenerateFloat(Rng,
                                             PixelUBase,
                                             PixelUBase + PixelUIncrement,
                                             &PixelU);

                RandomReferenceGenerateFloat(Rng,
                                             PixelVBase,
                                             PixelVBase + PixelVIncrement,
                                             &PixelV);

                RandomReferenceGenerateFloat(Rng,
                                             LensUBase,
                                             LensUBase + LensUIncrement,
                                             &LensU);

                RandomReferenceGenerateFloat(Rng,
                                             LensVBase,
                                             LensVBase + LensVIncrement,
                                             &LensV);
            }
            else
            {
                PixelU = PixelUBase + PixelUIncrement * (FLOAT) 0.5f;
                PixelV = PixelVBase + PixelVIncrement * (FLOAT) 0.5f;
                LensU = LensUBase + LensUIncrement * (FLOAT) 0.5f;
                LensV = LensVBase + LensVIncrement * (FLOAT) 0.5f;
            }

            RayGeneratorGenerateRay(RayGenerator,
                                    PixelU,
                                    PixelV,
                                    LensU,
                                    LensV,
                                    &WorldRay);

            Status = SampleTracerTraceRay(SampleTracer,
                                          WorldRay,
                                          Rng,
                                          &SampleColor);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }

            PixelColor = Color3Add(PixelColor, SampleColor);

            PixelVBase += PixelVIncrement;
            LensVBase += LensVIncrement;
        }

        PixelUBase += PixelUIncrement;
        LensUBase += LensUIncrement;
    }

    *Color = Color3ScaleByScalar(PixelColor, GridPixelSampler->SampleWeight);

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC PIXEL_SAMPLER_VTABLE GridPixelSamplerVTable = {
    GridPixelSamplerSamplePixel,
    NULL
};

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERATOOLKITAPI
ISTATUS
GridPixelSamplerAllocate(
    _In_ UINT32 XSamplesPerPixel,
    _In_ UINT32 YSamplesPerPixel,
    _In_ BOOL Jitter,
    _Out_ PPIXEL_SAMPLER *PixelSampler
    )
{
    GRID_PIXEL_SAMPLER GridPixelSampler;
    UINT32 TotalSamplesPerPixel;
    ISTATUS Status;

    if (XSamplesPerPixel == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (XSamplesPerPixel == 1)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (PixelSampler == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = CheckedMultiplyUInt32(XSamplesPerPixel, 
                                   YSamplesPerPixel,
                                   &TotalSamplesPerPixel);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    GridPixelSampler.XSamplesPerPixel = XSamplesPerPixel;
    GridPixelSampler.YSamplesPerPixel = YSamplesPerPixel;
    GridPixelSampler.Jitter = Jitter;
    GridPixelSampler.SampleWeight = (FLOAT) 1.0f / (FLOAT) TotalSamplesPerPixel;

    Status = PixelSamplerAllocate(&GridPixelSamplerVTable,
                                  &GridPixelSampler,
                                  sizeof(GRID_PIXEL_SAMPLER),
                                  _Alignof(GRID_PIXEL_SAMPLER),
                                  PixelSampler);
    
    return Status;
}