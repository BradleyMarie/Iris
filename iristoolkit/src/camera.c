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
ISTATUS
PinholeCameraRender(
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

    if (RowsToRender == 0)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    FramebufferGetDimensions(Framebuffer, 
                             &FramebufferRows,
                             &FramebufferColumns);

    LastRowToRender = StartingRow + RowsToRender;

    if (FramebufferRows < LastRowToRender)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    PointVectorAddShrunk(ImagePlaneStartingLocation,
                         PixelXDimensions,
                         (FLOAT) 1.0 + (FLOAT) AdditionalXSamplesPerPixel,
                         &RowStart);

    PointVectorAddShrunk(ImagePlaneStartingLocation,
                         PixelYDimensions,
                         (FLOAT) 1.0 + (FLOAT) AdditionalYSamplesPerPixel,
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

            PointVectorAdd(&Origin, PixelYDimensions, &Origin);   
        }

        PointVectorAdd(&RowStart, PixelYDimensions, &RowStart);
    }

    return ISTATUS_SUCCESS;
}