/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    camera.c

Abstract:

    This file contains the definitions for the camera routines.

--*/

#include <iristoolkitp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
StaticPinholeCameraRender(
    _In_ PCPOINT3 PinholeLocation,
    _In_ PCPOINT3 ImagePlaneStartingLocation,
    _In_ PCVECTOR3 PixelXDimensions,
    _In_ PCVECTOR3 PixelYDimensions,
    _In_ SIZE_T StartingRow,
    _In_ SIZE_T RowsToRender,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ BOOL Jitter,
    _Inout_opt_ PRANDOM Rng,
    _Inout_ PTRACER RayTracer,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    SIZE_T FramebufferColumnSubpixelIndex;
    SIZE_T FramebufferRowSubpixelIndex;
    SIZE_T FramebufferColumnIndex;
    SIZE_T FramebufferRowIndex;
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    SIZE_T LastRowToRender;
    POINT3 SubpixelRowStart;
    POINT3 SubpixelOrigin;
    FLOAT RandomNumber0;
    FLOAT RandomNumber1;
    FLOAT SubpixelWeight;
    COLOR3 SubpixelColor;
    COLOR3 PixelColor;
    VECTOR3 SubpixelXDimensions;
    VECTOR3 SubpixelYDimensions;
    VECTOR3 Direction;
    POINT3 RayOrigin;
    POINT3 RowStart;
    ISTATUS Status;
    POINT3 Origin;
    RAY WorldRay;

    ASSERT(PinholeLocation != NULL);
    ASSERT(ImagePlaneStartingLocation != NULL);
    ASSERT(PixelXDimensions != NULL);
    ASSERT(PixelYDimensions != NULL);
    ASSERT(RowsToRender != 0);
    ASSERT(Jitter == FALSE || Rng != NULL);
    ASSERT(RayTracer != NULL);
    ASSERT(Framebuffer != NULL);

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    LastRowToRender = StartingRow + RowsToRender;

    ASSERT(LastRowToRender <= FramebufferRows);

    if (AdditionalXSamplesPerPixel != 0)
    {
        VectorScale(PixelXDimensions,
                    (FLOAT) 1.0 / (FLOAT) AdditionalXSamplesPerPixel,
                    &SubpixelXDimensions);
    }
    else
    {
        SubpixelXDimensions = *PixelXDimensions;
    }

    if (AdditionalYSamplesPerPixel != 0)
    {
        VectorScale(PixelYDimensions,
                    (FLOAT) 1.0 / (FLOAT) AdditionalYSamplesPerPixel,
                    &SubpixelYDimensions);
    }
    else
    {
        SubpixelYDimensions = *PixelYDimensions;
    }

    SubpixelWeight = (FLOAT) 1.0 /
                      (((FLOAT) 1.0 + (FLOAT)AdditionalXSamplesPerPixel) *
                      ((FLOAT)AdditionalYSamplesPerPixel + (FLOAT) 1.0));

    RowStart = *ImagePlaneStartingLocation;

    for (FramebufferRowIndex = StartingRow;
         FramebufferRowIndex < LastRowToRender;
         FramebufferRowIndex++)
    {
        Origin = RowStart;

        for (FramebufferColumnIndex = 0;
             FramebufferColumnIndex < FramebufferColumns;
             FramebufferColumnIndex++)
        {
            Color3InitializeBlack(&PixelColor);

            SubpixelRowStart = Origin;

            FramebufferRowSubpixelIndex = 0;

            do
            {
                SubpixelOrigin = SubpixelRowStart;

                FramebufferColumnSubpixelIndex = 0;

                do
                {
                    RayOrigin = SubpixelOrigin;

                    if (Jitter != FALSE)
                    {
                        RandomNumber0 = RandomGenerateFloat(Rng, (FLOAT) 0.0, (FLOAT) 1.0);
                        RandomNumber1 = RandomGenerateFloat(Rng, (FLOAT) 0.0, (FLOAT) 1.0);

                        PointVectorAddScaled(&RayOrigin,
                                             &SubpixelXDimensions,
                                             RandomNumber0,
                                             &RayOrigin);

                        PointVectorAddScaled(&RayOrigin,
                                             &SubpixelYDimensions,
                                             RandomNumber1,
                                             &RayOrigin);
                    }
                    else
                    {
                        PointVectorAddScaled(&RayOrigin,
                                             &SubpixelXDimensions,
                                             (FLOAT) 0.5,
                                             &RayOrigin);

                        PointVectorAddScaled(&RayOrigin,
                                             &SubpixelYDimensions,
                                             (FLOAT) 0.5,
                                             &RayOrigin);
                    }

                    PointSubtract(&RayOrigin, PinholeLocation, &Direction);

                    VectorNormalize(&Direction, &Direction);

                    RayInitialize(&WorldRay, &RayOrigin, &Direction);

                    Status = TracerTraceRay(RayTracer,
                                            &WorldRay,
                                            &SubpixelColor);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        return Status;
                    }

                    Color3Add(&PixelColor, &SubpixelColor, &PixelColor);

                    PointVectorAdd(&SubpixelOrigin, &SubpixelXDimensions, &SubpixelOrigin);

                } while (++FramebufferColumnSubpixelIndex < AdditionalXSamplesPerPixel);

                PointVectorAdd(&SubpixelRowStart, &SubpixelYDimensions, &SubpixelRowStart);

            } while (++FramebufferRowSubpixelIndex < AdditionalYSamplesPerPixel);

            Color3ScaleByScalar(&PixelColor,
                                SubpixelWeight,
                                &PixelColor);

            FramebufferSetPixel(Framebuffer,
                                &PixelColor,
                                FramebufferRowIndex,
                                FramebufferColumnIndex);

            PointVectorAdd(&Origin, PixelXDimensions, &Origin);
        }

        PointVectorAdd(&RowStart, PixelYDimensions, &RowStart);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PinholeCameraRender(
    _In_ PCPOINT3 PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ PCVECTOR3 CameraDirection,
    _In_ PCVECTOR3 Up,
    _In_ SIZE_T StartingRow,
    _In_ SIZE_T RowsToRender,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ BOOL Jitter,
    _Inout_opt_ PRANDOM Rng,
    _Inout_ PTRACER RayTracer,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    VECTOR3 NormalizedCameraDirection;
    VECTOR3 NormalizedUpVector;
    VECTOR3 ImagePlaneHeightVector;
    VECTOR3 ImagePlaneWidthVector;
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    SIZE_T LastRowToRender;
    POINT3 ImagePlaneCorner;
    ISTATUS Status;

    if (PinholeLocation == NULL ||
        IsNormalFloat(ImagePlaneDistance) == FALSE ||
        IsFiniteFloat(ImagePlaneDistance) == FALSE ||
        IsZeroFloat(ImagePlaneDistance) != FALSE ||
        IsNormalFloat(ImagePlaneHeight) == FALSE ||
        IsFiniteFloat(ImagePlaneHeight) == FALSE ||
        IsZeroFloat(ImagePlaneHeight) != FALSE ||
        IsNormalFloat(ImagePlaneWidth) == FALSE ||
        IsFiniteFloat(ImagePlaneWidth) == FALSE ||
        IsZeroFloat(ImagePlaneWidth) != FALSE ||
        CameraDirection == NULL ||
        Up == NULL ||
        RowsToRender == 0 ||
        (Jitter == TRUE && Rng == NULL) ||
        RayTracer == NULL ||
        Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    if (FramebufferRows < StartingRow)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    LastRowToRender = StartingRow + RowsToRender;

    if (FramebufferRows < LastRowToRender)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    VectorNormalize(CameraDirection, &NormalizedCameraDirection);

    VectorNormalize(Up, &NormalizedUpVector);

    VectorCrossProduct(&NormalizedCameraDirection,
                       Up,
                       &ImagePlaneWidthVector);

    VectorNormalize(&ImagePlaneWidthVector, &ImagePlaneWidthVector);

    VectorCrossProduct(CameraDirection,
                       &ImagePlaneWidthVector,
                       &ImagePlaneHeightVector);

    VectorNormalize(&ImagePlaneHeightVector, &ImagePlaneHeightVector);

    VectorScale(&ImagePlaneWidthVector,
                ImagePlaneWidth,
                &ImagePlaneWidthVector);

    VectorScale(&ImagePlaneHeightVector,
                ImagePlaneHeight,
                &ImagePlaneHeightVector);

    PointVectorAddScaled(PinholeLocation,
                         &NormalizedCameraDirection,
                         ImagePlaneDistance,
                         &ImagePlaneCorner);

    PointVectorSubtractScaled(&ImagePlaneCorner,
                              &ImagePlaneWidthVector,
                              (FLOAT) 0.5,
                              &ImagePlaneCorner);

    PointVectorSubtractScaled(&ImagePlaneCorner,
                              &ImagePlaneHeightVector,
                              (FLOAT) 0.5,
                              &ImagePlaneCorner);

    VectorScale(&ImagePlaneWidthVector,
                (FLOAT) 1.0 / (FLOAT) FramebufferColumns,
                &ImagePlaneWidthVector);

    VectorScale(&ImagePlaneHeightVector,
                (FLOAT) 1.0 / (FLOAT) FramebufferRows,
                &ImagePlaneHeightVector);

    Status = StaticPinholeCameraRender(PinholeLocation,
                                       &ImagePlaneCorner,
                                       &ImagePlaneWidthVector,
                                       &ImagePlaneHeightVector,
                                       StartingRow,
                                       RowsToRender,
                                       AdditionalXSamplesPerPixel,
                                       AdditionalYSamplesPerPixel,
                                       Jitter,
                                       Rng,
                                       RayTracer,
                                       Framebuffer);

    return Status;
}