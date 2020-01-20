/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    pfm.h

Abstract:

    Tests if a framebuffer matches a pfm file.

--*/

#ifndef _TEST_UTIL_PFM_
#define _TEST_UTIL_PFM_

#include "iris_camera/iris_camera.h"
#include "iris_camera_toolkit/pfm_writer.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ExactlyEqualsPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format,
    _Out_ bool *result
    );

ISTATUS
ApproximatelyEqualsPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format,
    _In_ float_t epsilon,
    _Out_ bool *result
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _TEST_UTIL_PFM_