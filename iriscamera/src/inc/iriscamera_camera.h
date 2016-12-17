/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_camera.h

Abstract:

    This file contains the internal definitions for the CAMERA type.

--*/

#ifndef _CAMERA_IRIS_CAMERA_INTERNAL_
#define _CAMERA_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Functions
//

VOID
CameraGetParameters(
    _In_ PCCAMERA Camera,
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
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraComputeRay(
    _In_ PCCAMERA Camera,
    _In_range_(0, NumberOfRows) SIZE_T Row,
    _In_ SIZE_T NumberOfRows,
    _In_range_(0, NumberOfColumns) SIZE_T Column,
    _In_ SIZE_T NumberOfColumns,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    );

#endif // _CAMERA_IRIS_CAMERA_INTERNAL_