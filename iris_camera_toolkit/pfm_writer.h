/*++

Copyright (c) 2018 Brad Weinberger

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

typedef enum _PFM_PIXEL_FORMAT {
    PFM_PIXEL_FORMAT_XYZ,
} PFM_PIXEL_FORMAT;

ISTATUS
WriteToPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_CAMERA_TOOLKIT_PFM_WRITER_