/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    pinhole_camera.h

Abstract:

    Creates a pinhole camera.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_PINHOLE_CAMERA_
#define _IRIS_CAMERA_TOOLKIT_PINHOLE_CAMERA_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PinholeCameraAllocate(
    _In_ POINT3 location,
    _In_ VECTOR3 direction,
    _In_ VECTOR3 up,
    _In_ float_t focal_length,
    _In_ float_t frame_width,
    _In_ float_t frame_height,
    _Out_ PCAMERA *camera
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_PINHOLE_CAMERA_