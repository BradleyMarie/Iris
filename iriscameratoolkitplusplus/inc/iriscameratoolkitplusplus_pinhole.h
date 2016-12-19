/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkitplusplus_pinhole.h

Abstract:

    This file contains the definition for the PinholeCamera type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _PINHOLE_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
#define _PINHOLE_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_

namespace IrisCameraToolkit {
namespace PinholeCamera {

//
// Functions
//

static
inline
IrisCamera::Camera
Create(
    _In_ const Iris::Point & PinholeLocation,
    _In_ const Iris::Vector & CameraDirection,
    _In_ const Iris::Vector & Up,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth
    )
{
    PCAMERA CameraPtr;
    ISTATUS Status = PinholeCameraAllocate(PinholeLocation.AsPOINT3(),
                                           CameraDirection.AsVECTOR3(),
                                           Up.AsVECTOR3(),
                                           ImagePlaneDistance,
                                           ImagePlaneHeight,
                                           ImagePlaneWidth,
                                           &CameraPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("PinholeLocation");
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("CameraDirection");
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Up");
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("ImagePlaneDistance");
        case ISTATUS_INVALID_ARGUMENT_04:
            throw std::invalid_argument("ImagePlaneHeight");
        case ISTATUS_INVALID_ARGUMENT_05:
            throw std::invalid_argument("ImagePlaneWidth");
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisCamera::Camera(CameraPtr);
}

} // namespace PinholeCamera
} // namespace IrisCameraToolkit

#endif // _PINHOLE_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
