/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_framebuffer.h

Abstract:

    This file contains the prototypes for the FRAMEBUFFER type.

--*/

#ifndef _FRAMEBUFFER_IRIS_CAMERA_
#define _FRAMEBUFFER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef struct _FRAMEBUFFER FRAMEBUFFER, *PFRAMEBUFFER;
typedef CONST FRAMEBUFFER *PCFRAMEBUFFER;

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
FramebufferAllocate(
    _In_ COLOR3 InitialColor,
    _In_ SIZE_T Rows,
    _In_ SIZE_T Columns,
    _Out_ PFRAMEBUFFER *FrameBuffer
    );

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
FramebufferSetPixel(
    _Inout_ PFRAMEBUFFER Framebuffer,
    _In_ COLOR3 Color,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column
    );

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
FramebufferGetPixel(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column,
    _Out_ PCOLOR3 Color
    );

IRISCAMERAAPI
ISTATUS
FramebufferGetDimensions(
    _In_ PCFRAMEBUFFER Framebuffer,
    _Out_ PSIZE_T Rows,
    _Out_ PSIZE_T Columns
    );

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
FramebufferSaveAsPFM(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ PCSTR Path
    );

IRISCAMERAAPI
VOID
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER Framebuffer
    );

#endif // _FRAMEBUFFER_IRIS_CAMERA_
