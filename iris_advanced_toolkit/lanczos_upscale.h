/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    lanczos_upscale.h

Abstract:

    Upscale an image to be the next power of two in each dimension using
    Lanczos resampling.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_LANCZOS_UPSCALE_
#define _IRIS_ADVANCED_TOOLKIT_LANCZOS_UPSCALE_

#include "iris_advanced/iris_advanced.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LanczosUpscaleColors(
    _In_reads_(height * width) _Post_invalid_ COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) PCOLOR3 *new_texels,
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    );

ISTATUS
LanczosUpscaleFloats(
    _In_reads_(height * width) _Post_invalid_ float_t texels[],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) float_t **new_texels,
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_ADVANCED_TOOLKIT_LANCZOS_UPSCALE_