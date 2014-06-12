/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_framebuffer.h

Abstract:

    This file contains the prototypes for the FRAMEBUFFER type.

--*/

#ifndef _FRAMEBUFFER_IRIS_TOOLKIT_
#define _FRAMEBUFFER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef struct _FRAMEBUFFER FRAMEBUFFER, *PFRAMEBUFFER;
typedef CONST FRAMEBUFFER *PCFRAMEBUFFER;

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PFRAMEBUFFER
FramebufferAllocate(
    _In_ PCCOLOR3 InitialColor,
    _In_ SIZE_T Rows,
    _In_ SIZE_T Columns
    );

_Success_(return == ISTATUS_SUCCESS)
IRISTOOLKITAPI
ISTATUS
FramebufferSetPixel(
    _Inout_ PFRAMEBUFFER Framebuffer,
    _In_ PCCOLOR3 Color,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column
    );

_Success_(return == ISTATUS_SUCCESS)
IRISTOOLKITAPI
ISTATUS
FramebufferGetPixel(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column,
    _Out_ PCOLOR3 Color
    );

IRISTOOLKITAPI
ISTATUS
FramebufferGetDimensions(
    _In_ PCFRAMEBUFFER Framebuffer,
    _Out_ PSIZE_T Rows,
    _Out_ PSIZE_T Columns
    );

IRISTOOLKITAPI
VOID
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER Framebuffer
    );

#endif // _FRAMEBUFFER_IRIS_TOOLKIT_