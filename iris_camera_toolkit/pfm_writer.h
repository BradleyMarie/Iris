/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    pfm_writer.h

Abstract:

    Writes a framebuffer out to a pfm file.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_PFM_WRITER_
#define _IRIS_CAMERA_TOOLKIT_PFM_WRITER_

#include "iris_camera/iris_camera.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
WriteToPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_z_ const char* filename,
    _In_ COLOR_SPACE color_space
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_PFM_WRITER_