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
    : Data(CameraPtr)
    { 
        if (CameraPtr == nullptr)
        {
            throw std::invalid_argument("CameraPtr");
        }
    }

    Camera(
        _In_ Camera && ToMove
        )
    : Data(ToMove.Data)
    { 
        ToMove.Data = nullptr;
    }

    _Ret_
    PCAMERA
    AsPCAMERA(
        void
        )
    {
        return Data;
    }

    _Ret_
    PCCAMERA
    AsPCCAMERA(
        void
        ) const
    {
        return Data;
    }

    Camera(
        _In_ const Camera & ToCopy
        ) = delete;
        
    Camera &
    operator=(
        _In_ const Camera & ToCopy
        ) = delete;

    ~Camera(
        void
        )
    {
        CameraFree(Data);
    }

private:
    PCAMERA Data;
};

} // namespace IrisCamera

#endif // _CAMERA_IRIS_CAMERA_PLUS_PLUS_HEADER_
