/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_pixelsampler.h

Abstract:

    This file contains the definitions for the PIXEL_SAMPLER type.

--*/

#ifndef _PIXEL_SAMPLER_IRIS_CAMERA_
#define _PIXEL_SAMPLER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPIXEL_SAMPLER_GENERATE_RAY_CALLBACK)(
    _In_ PVOID Context,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _In_ FLOAT Time,
    _Out_ PRAY WorldRay
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPIXEL_SAMPLER_RENDER_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PPIXEL_SAMPLER_RENDER_CALLBACK Callback,
    _In_ PCVOID CallbackContext,
    _Inout_ PFRAMEBUFFER Framebuffer
    );

typedef struct _PIXEL_SAMPLER_VTABLE {
    PPIXEL_SAMPLER_RENDER_ROUTINE RenderRoutine;
    PFREE_ROUTINE FreeRoutine;
} PIXEL_SAMPLER_VTABLE, *PPIXEL_SAMPLER_VTABLE;

typedef CONST PIXEL_SAMPLER_VTABLE *PCPIXEL_SAMPLER_VTABLE;

typedef struct _PIXEL_SAMPLER PIXEL_SAMPLER, *PPIXEL_SAMPLER;
typedef CONST PIXEL_SAMPLER *PCPIXEL_SAMPLER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
PixelSamplerAllocate(
    _In_ PCPIXEL_SAMPLER_VTABLE PixelSamplerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPIXEL_SAMPLER *PixelSampler
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
PixelSamplerRender(
    _In_ PCPIXEL_SAMPLER PixelSampler,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column,
    _In_ SIZE_T NumberOfRows,
    _In_ SIZE_T NumberOfColumns
    _In_ PPIXEL_SAMPLER_RENDER_CALLBACK Callback,
    _In_ PCVOID CallbackContext,
    _Out_ PCOLOR3 Color
    );

IRISCAMERAAPI
VOID
PixelSamplerFree(
    _In_opt_ _Post_invalid_ PPIXEL_SAMPLER PixelSampler
    );

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_