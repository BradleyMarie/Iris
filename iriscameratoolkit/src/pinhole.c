/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    pinhole.c

Abstract:

    This file contains the definitions for the PINHOLE_CAMERA type.

--*/

#include <iriscameratoolkit.h>

//
// Types
//

typedef struct _PINHOLE_CAMERA {
    POINT3 PinholeLocation;
    POINT3 ImagePlaneCorner;
    VECTOR3 ImagePlaneHeightVector;
    VECTOR3 ImagePlaneWidthVector;
} PINHOLE_CAMERA, *PPINHOLE_CAMERA;

typedef CONST PINHOLE_CAMERA *PCPINHOLE_CAMERA;

//
// Static functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PinholeComputeRay(
    _In_ PCVOID Context,
    _In_range_(0, NumberOfRows - 1) SIZE_T Row,
    _In_ SIZE_T NumberOfRows,
    _In_range_(0, NumberOfColumns - 1) SIZE_T Column,
    _In_ SIZE_T NumberOfColumns,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    )
{
    VECTOR3 Direction;
    POINT3 Origin;
    PCPINHOLE_CAMERA PinholeCamera;

    ASSERT(Context != NULL);
    ASSERT(Row < NumberOfRows);
    ASSERT(Column < NumberOfRows);
    ASSERT(WorldRay != NULL);

    if (IsFiniteFloat(PixelU) == FALSE ||
        IsLessThanZeroFloat(PixelU) != FALSE ||
        (FLOAT) 1.0f < PixelU)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (IsFiniteFloat(PixelV) == FALSE ||
        IsLessThanZeroFloat(PixelV) != FALSE ||
        (FLOAT) 1.0f < PixelV)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (IsFiniteFloat(LensU) == FALSE ||
        IsLessThanZeroFloat(LensU) != FALSE ||
        (FLOAT) 1.0f < LensU)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (IsFiniteFloat(LensV) == FALSE ||
        IsLessThanZeroFloat(LensV) != FALSE ||
        (FLOAT) 1.0f < LensV)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    PinholeCamera = (PCPINHOLE_CAMERA) Context;

    Origin = PointVectorAddScaled(PinholeCamera->ImagePlaneCorner, 
                                  PinholeCamera->ImagePlaneHeightVector, 
                                  ((FLOAT) Column + PixelU) / (FLOAT) NumberOfColumns);

    Origin = PointVectorAddScaled(Origin, 
                                  PinholeCamera->ImagePlaneWidthVector, 
                                  ((FLOAT) Row + PixelV) / (FLOAT) NumberOfRows);

    Direction = PointSubtract(Origin, 
                              PinholeCamera->PinholeLocation);

    Direction = VectorNormalize(Direction,
                                NULL, 
                                NULL);

    *WorldRay = RayCreate(Origin, 
                          Direction);

    return ISTATUS_SUCCESS;
}

STATIC
VOID
PinholeGetParameters(
    _In_ PCVOID Context,
    _Out_ PBOOL SamplePixel,
    _Out_ PBOOL SampleLens,
    _Out_ PFLOAT MinPixelU,
    _Out_ PFLOAT MaxPixelU,
    _Out_ PFLOAT MinPixelV,
    _Out_ PFLOAT MaxPixelV,
    _Out_ PFLOAT MinLensU,
    _Out_ PFLOAT MaxLensU,
    _Out_ PFLOAT MinLensV,
    _Out_ PFLOAT MaxLensV
    )
{
    ASSERT(Context != NULL);
    ASSERT(SamplePixel != NULL);
    ASSERT(SampleLens != NULL);
    ASSERT(MinPixelU != NULL);
    ASSERT(MaxPixelU != NULL);
    ASSERT(MinPixelV != NULL);
    ASSERT(MaxPixelV != NULL);
    ASSERT(MinLensU != NULL);
    ASSERT(MaxLensU != NULL);
    ASSERT(MinLensV != NULL);
    ASSERT(MaxLensV != NULL);

    *SamplePixel = TRUE;
    *SampleLens = FALSE;
    *MinPixelU = (FLOAT) 0.0f;
    *MaxPixelU = (FLOAT) 1.0f;
    *MinPixelV = (FLOAT) 0.0f;
    *MaxPixelV = (FLOAT) 1.0f;
    *MinLensU = (FLOAT) 0.0f;
    *MaxLensU = (FLOAT) 1.0f;
    *MinLensV = (FLOAT) 0.0f;
    *MaxLensV = (FLOAT) 1.0f;
}

//
// Static variables
//

CONST STATIC CAMERA_VTABLE PinholeVTable = {
    PinholeComputeRay,
    PinholeGetParameters,
    NULL
};

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PinholeCameraAllocate(
    _In_ POINT3 PinholeLocation,
    _In_ VECTOR3 CameraDirection,
    _In_ VECTOR3 Up,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _Out_ PCAMERA *Camera
    )
{
    POINT3 ImagePlaneCorner;
    VECTOR3 ImagePlaneHeightVector;
    VECTOR3 ImagePlaneWidthVector;
    VECTOR3 NormalizedCameraDirection;
    VECTOR3 NormalizedUpVector;

    PINHOLE_CAMERA PinholeCamera;
    ISTATUS Status;

    if (PointValidate(PinholeLocation) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(CameraDirection) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(Up) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (IsFiniteFloat(ImagePlaneDistance) == FALSE ||
        IsGreaterThanZeroFloat(ImagePlaneDistance) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (IsFiniteFloat(ImagePlaneHeight) == FALSE ||
        IsGreaterThanZeroFloat(ImagePlaneHeight) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsFiniteFloat(ImagePlaneWidth) == FALSE ||
        IsGreaterThanZeroFloat(ImagePlaneWidth) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    NormalizedCameraDirection = VectorNormalize(CameraDirection, 
                                                NULL, 
                                                NULL);

    NormalizedUpVector = VectorNormalize(Up, 
                                         NULL, 
                                         NULL);

    ImagePlaneWidthVector = VectorCrossProduct(NormalizedCameraDirection, 
                                               NormalizedUpVector);

    ImagePlaneWidthVector = VectorNormalize(ImagePlaneWidthVector, 
                                            NULL, 
                                            NULL);

    ImagePlaneHeightVector = VectorCrossProduct(CameraDirection,
                                                ImagePlaneWidthVector);

    ImagePlaneHeightVector = VectorNormalize(ImagePlaneHeightVector, 
                                             NULL, 
                                             NULL);

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

    PinholeCamera.PinholeLocation = PinholeLocation;
    PinholeCamera.ImagePlaneCorner = ImagePlaneCorner;
    PinholeCamera.ImagePlaneHeightVector = ImagePlaneHeightVector;
    PinholeCamera.ImagePlaneWidthVector = ImagePlaneWidthVector;

    Status = CameraAllocate(&PinholeVTable,
                            &PinholeCamera,
                            sizeof(PINHOLE_CAMERA),
                            _Alignof(PINHOLE_CAMERA),
                            Camera);

    return Status;
}
