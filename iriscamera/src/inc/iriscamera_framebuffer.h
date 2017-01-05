/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_framebuffer.h

Abstract:

    This file contains the internal prototypes for the FRAMEBUFFER type.

--*/

#ifndef _FRAMEBUFFER_IRIS_CAMERA_INTERNAL_
#define _FRAMEBUFFER_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
FramebufferAllocate(
    _In_ SIZE_T Rows,
    _In_ SIZE_T Columns,
    _Out_ PFRAMEBUFFER *FrameBuffer
    );

VOID
FramebufferSetPixel(
    _Inout_ PFRAMEBUFFER Framebuffer,
    _In_ COLOR3 Color,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column
    );

#endif // _FRAMEBUFFER_IRIS_CAMERA_INTERNAL_
