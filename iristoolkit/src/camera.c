/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    camera.c

Abstract:

    This file contains the definitions for the camera routines.

--*/

#include <iristoolkitp.h>

#define ISTATUS_INVALID_ARGUMENT     0x05 // Old Error

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
StaticPinholeCameraRender(
    _In_ POINT3 PinholeLocation,
    _In_ POINT3 ImagePlaneStartingLocation,
    _In_ VECTOR3 PixelXDimensions,
    _In_ VECTOR3 PixelYDimensions,
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
        SubpixelXDimensions = VectorScale(PixelXDimensions,
                                          (FLOAT) 1.0 / (FLOAT) AdditionalXSamplesPerPixel);
    }
    else
    {
        SubpixelXDimensions = PixelXDimensions;
    }

    if (AdditionalYSamplesPerPixel != 0)
    {
        SubpixelYDimensions = VectorScale(PixelYDimensions,
                                          (FLOAT) 1.0 / (FLOAT) AdditionalYSamplesPerPixel);
    }
    else
    {
        SubpixelYDimensions = PixelYDimensions;
    }

    SubpixelWeight = (FLOAT) 1.0 /
                      (((FLOAT) 1.0 + (FLOAT)AdditionalXSamplesPerPixel) *
                      ((FLOAT)AdditionalYSamplesPerPixel + (FLOAT) 1.0));

    RowStart = ImagePlaneStartingLocation;

    for (FramebufferRowIndex = StartingRow;
         FramebufferRowIndex < LastRowToRender;
         FramebufferRowIndex++)
    {
        Origin = RowStart;

        for (FramebufferColumnIndex = 0;
             FramebufferColumnIndex < FramebufferColumns;
             FramebufferColumnIndex++)
        {
            PixelColor = Color3InitializeBlack();

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
                        Status = RandomGenerateFloat(Rng,
                                                     (FLOAT) 0.0,
                                                     (FLOAT) 1.0,
                                                     &RandomNumber0);

                        if (Status != ISTATUS_SUCCESS)
                        {
                            return Status;
                        }

                        Status = RandomGenerateFloat(Rng,
                                                     (FLOAT) 0.0,
                                                     (FLOAT) 1.0,
                                                     &RandomNumber1);

                        if (Status != ISTATUS_SUCCESS)
                        {
                            return Status;
                        }

                        RayOrigin = PointVectorAddScaled(RayOrigin,
                                                         SubpixelXDimensions,
                                                         RandomNumber0);

                        RayOrigin = PointVectorAddScaled(RayOrigin,
                                                         SubpixelYDimensions,
                                                         RandomNumber1);
                    }
                    else
                    {
                        RayOrigin = PointVectorAddScaled(RayOrigin,
                                                         SubpixelXDimensions,
                                                         (FLOAT) 0.5);

                        RayOrigin = PointVectorAddScaled(RayOrigin,
                                                         SubpixelYDimensions,
                                                         (FLOAT) 0.5);
                    }

                    Direction = PointSubtract(RayOrigin, PinholeLocation);

                    Direction = VectorNormalize(Direction, NULL);

                    WorldRay = RayCreate(RayOrigin, Direction);

                    Status = TracerTraceRay(RayTracer,
                                            WorldRay,
                                            &SubpixelColor);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        return Status;
                    }

                    PixelColor = Color3Add(PixelColor, SubpixelColor);

                    SubpixelOrigin = PointVectorAdd(SubpixelOrigin, SubpixelXDimensions);

                } while (++FramebufferColumnSubpixelIndex < AdditionalXSamplesPerPixel);

                SubpixelRowStart = PointVectorAdd(SubpixelRowStart, SubpixelYDimensions);

            } while (++FramebufferRowSubpixelIndex < AdditionalYSamplesPerPixel);

            PixelColor = Color3ScaleByScalar(PixelColor,
                                             SubpixelWeight);

            FramebufferSetPixel(Framebuffer,
                                PixelColor,
                                FramebufferRowIndex,
                                FramebufferColumnIndex);

            Origin = PointVectorAdd(Origin, PixelXDimensions);
        }

        RowStart = PointVectorAdd(RowStart, PixelYDimensions);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PinholeCameraRender(
    _In_ POINT3 PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ VECTOR3 CameraDirection,
    _In_ VECTOR3 Up,
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

    if (PointValidate(PinholeLocation) == FALSE ||
        IsNormalFloat(ImagePlaneDistance) == FALSE ||
        IsFiniteFloat(ImagePlaneDistance) == FALSE ||
        IsZeroFloat(ImagePlaneDistance) != FALSE ||
        IsNormalFloat(ImagePlaneHeight) == FALSE ||
        IsFiniteFloat(ImagePlaneHeight) == FALSE ||
        IsZeroFloat(ImagePlaneHeight) != FALSE ||
        IsNormalFloat(ImagePlaneWidth) == FALSE ||
        IsFiniteFloat(ImagePlaneWidth) == FALSE ||
        IsZeroFloat(ImagePlaneWidth) != FALSE ||
        VectorValidate(CameraDirection) == FALSE ||
        VectorValidate(Up) == FALSE ||
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

    NormalizedCameraDirection = VectorNormalize(CameraDirection, NULL);

    NormalizedUpVector = VectorNormalize(Up, NULL);

    ImagePlaneWidthVector = VectorCrossProduct(NormalizedCameraDirection, Up);

    ImagePlaneWidthVector = VectorNormalize(ImagePlaneWidthVector, NULL);

    ImagePlaneHeightVector = VectorCrossProduct(CameraDirection,
                                                ImagePlaneWidthVector);

    ImagePlaneHeightVector = VectorNormalize(ImagePlaneHeightVector, NULL);

    ImagePlaneWidthVector = VectorScale(ImagePlaneWidthVector,
                                        ImagePlaneWidth);

    ImagePlaneHeightVector = VectorScale(ImagePlaneHeightVector,
                                         ImagePlaneHeight);

    ImagePlaneCorner = PointVectorAddScaled(PinholeLocation,
                                            NormalizedCameraDirection,
                                            ImagePlaneDistance);

    ImagePlaneCorner = PointVectorSubtractScaled(ImagePlaneCorner,
                                                 ImagePlaneWidthVector,
                                                 (FLOAT) 0.5);

    ImagePlaneCorner = PointVectorSubtractScaled(ImagePlaneCorner,
                                                 ImagePlaneHeightVector,
                                                 (FLOAT) 0.5);

    ImagePlaneWidthVector = VectorScale(ImagePlaneWidthVector,
                                        (FLOAT) 1.0 / (FLOAT) FramebufferColumns);

    ImagePlaneHeightVector = VectorScale(ImagePlaneHeightVector,
                                         (FLOAT) 1.0 / (FLOAT) FramebufferRows);

    Status = StaticPinholeCameraRender(PinholeLocation,
                                       ImagePlaneCorner,
                                       ImagePlaneWidthVector,
                                       ImagePlaneHeightVector,
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