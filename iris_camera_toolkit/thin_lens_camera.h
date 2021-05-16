/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    thin_lens_camera.h

Abstract:

    Creates a camera with a thin lens and a circular aperture.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_THIN_LENS_CAMERA_
#define _IRIS_CAMERA_TOOLKIT_THIN_LENS_CAMERA_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ThinLensCameraAllocate(
    _In_ POINT3 location,
    _In_ VECTOR3 direction,
    _In_ VECTOR3 up,
    _In_ float_t focal_length,
    _In_ float_t f_number,
    _In_ float_t frame_width,
    _In_ float_t frame_height,
    _Out_ PCAMERA *camera
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_THIN_LENS_CAMERA_