/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_camera.h

Abstract:

    This file contains the definitions for the CAMERA type.

--*/

#ifndef _CAMERA_IRIS_CAMERA_
#define _CAMERA_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PCAMERA_RENDER_CALLBACK)(
    _In_ PVOID Context,
    _In_ RAY WorldRay,
    _Out_ PCOLOR3 Color
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PCAMERA_RENDER_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCAMERA_RENDER_CALLBACK Callback,
    _In_ PCVOID CallbackContext,
    _Inout_ PFRAMEBUFFER Framebuffer
    );

typedef struct _CAMERA_VTABLE {
    PCAMERA_RENDER_ROUTINE RenderRoutine;
    PFREE_ROUTINE FreeRoutine;
} CAMERA_VTABLE, *PCAMERA_VTABLE;

typedef CONST CAMERA_VTABLE *PCCAMERA_VTABLE;

typedef struct _CAMERA CAMERA, *PCAMERA;
typedef CONST CAMERA *PCCAMERA;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
CameraAllocate(
    _In_ PCCAMERA_VTABLE CameraVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCAMERA *Camera
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
CameraRender(
    _In_ PCCAMERA Camera,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column,
    _In_ SIZE_T NumberOfRows,
    _In_ SIZE_T NumberOfColumns,
    _In_ PSAMPLER_2D PixelSampler,
    _In_ PSAMPLER_2D LensSampler,
    _In_ PCAMERA_RENDER_CALLBACK Callback,
    _In_ PCVOID CallbackContext,
    _Out_ PCOLOR3 Color
    );

IRISCAMERAAPI
VOID
CameraFree(
    _In_opt_ _Post_invalid_ PCAMERA Camera
    );

#endif // _CAMERA_IRIS_CAMERA_