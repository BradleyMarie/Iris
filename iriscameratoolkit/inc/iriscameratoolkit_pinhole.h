/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkit_pinhole.h

Abstract:

    This file contains the pinhole camera routines.

--*/

#ifndef _PINHOLE_IRIS_CAMERA_TOOLKIT_
#define _PINHOLE_IRIS_CAMERA_TOOLKIT_

#include <iriscameratoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERATOOLKITAPI
ISTATUS
PinholeCameraAllocate(
    _In_ POINT3 PinholeLocation,
    _In_ VECTOR3 CameraDirection,
    _In_ VECTOR3 Up,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _Out_ PCAMERA *Camera
    );

#endif // _PINHOLE_IRIS_CAMERA_TOOLKIT_