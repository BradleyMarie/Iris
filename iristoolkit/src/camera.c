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
    _Inout_ PRAYSHADER RayShader,
    _Inout_ PFRAMEBUFFER Framebuffer
    )
{
    SIZE_T FramebufferColumnIndex;
    SIZE_T FramebufferRowIndex;
    SIZE_T FramebufferColumns;
    SIZE_T FramebufferRows;
    SIZE_T LastRowToRender;
    VECTOR3 Direction;
    POINT3 RowStart;
    ISTATUS Status;
    POINT3 Origin;
    COLOR3 Color;
    RAY WorldRay;

    ASSERT(PinholeLocation != NULL);
    ASSERT(ImagePlaneStartingLocation != NULL);
    ASSERT(PixelXDimensions != NULL);
    ASSERT(PixelYDimensions != NULL);
    ASSERT(RowsToRender != 0);
    ASSERT(Jitter == FALSE || Rng != NULL);
    ASSERT(RayShader != NULL);
    ASSERT(Framebuffer != NULL);

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    LastRowToRender = StartingRow + RowsToRender;

    ASSERT(LastRowToRender <= FramebufferRows);

    PointVectorAddScaled(ImagePlaneStartingLocation,
                         PixelXDimensions,
                         (FLOAT) 1.0 / ((FLOAT) 1.0 + (FLOAT) AdditionalXSamplesPerPixel),
                         &RowStart);

    PointVectorAddScaled(ImagePlaneStartingLocation,
                         PixelYDimensions,
                         (FLOAT) 1.0 / ((FLOAT) 1.0 + (FLOAT) AdditionalYSamplesPerPixel),
                         &RowStart);

    for (FramebufferRowIndex = StartingRow;
         FramebufferRowIndex < LastRowToRender;
         FramebufferRowIndex++)
    {
        Origin = RowStart;

        for (FramebufferColumnIndex = 0;
             FramebufferColumnIndex < FramebufferColumns;
             FramebufferColumnIndex++)
        {
            PointSubtract(&Origin, PinholeLocation, &Direction);

            VectorNormalize(&Direction, &Direction);

            RayInitialize(&WorldRay, &Origin, &Direction);

            Status = RayShaderTraceRay(RayShader,
                                       &WorldRay,
                                       &Color);

            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }

            FramebufferSetPixel(Framebuffer,
                                &Color,
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
    _Inout_ PRAYSHADER RayShader,
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
        RayShader == NULL ||
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
                                       RayShader,
                                       Framebuffer);

    return Status;
}