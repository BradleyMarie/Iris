/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_camera.h

Abstract:

    This file contains the prototypes for the camera routines.

--*/

#ifndef _CAMERA_IRIS_TOOLKIT_
#define _CAMERA_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISTOOLKITAPI
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
    );

#endif // _CAMERA_IRIS_TOOLKIT_