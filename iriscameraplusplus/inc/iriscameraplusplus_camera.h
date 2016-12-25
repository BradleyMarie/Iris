/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_camera.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ Camera type.

--*/

#include <iriscameraplusplus.h>

#ifndef _CAMERA_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _CAMERA_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class Camera final {
public:
    Camera(
        _In_ PCAMERA CameraPtr
        )
    : Data(CameraPtr, [](PCAMERA ToFree){ CameraFree(ToFree); })
    { 
        if (CameraPtr == nullptr)
        {
            throw std::invalid_argument("CameraPtr");
        }
    }

    _Ret_
    PCCAMERA
    AsPCCAMERA(
        void
        ) const
    {
        return Data.get();
    }

private:
    std::shared_ptr<CAMERA> Data;
};

} // namespace IrisCamera

#endif // _CAMERA_IRIS_CAMERA_PLUS_PLUS_HEADER_
