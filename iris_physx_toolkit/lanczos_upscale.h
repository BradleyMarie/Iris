/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    lanczos_upscale.h

Abstract:

    Upscale an image to be the next power of two in each dimension using
    Lanczos resampling.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_LANCZOS_UPSCALE_
#define _IRIS_PHYSX_TOOLKIT_LANCZOS_UPSCALE_

#include "iris_advanced/iris_advanced.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
LancsozUpscaleBytes(
    _In_reads_(height * width) _Post_invalid_ unsigned char texels[],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) unsigned char **new_texels,
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    );

ISTATUS
LanczosUpscaleByteTuples(
    _In_reads_(height * width) _Post_invalid_ unsigned char texels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) unsigned char (**new_texels)[3],
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    );

ISTATUS
LanczosUpscaleFloatTuples(
    _In_reads_(height * width) _Post_invalid_ float_t texels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _Outptr_result_buffer_(*new_width * *new_height) float_t (**new_texels)[3],
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_LANCZOS_UPSCALE_