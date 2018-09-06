/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    framebuffer.h

Abstract:

    A container for holding an RGB image.

--*/

#ifndef _IRIS_CAMERA_FRAMEBUFFER_
#define _IRIS_CAMERA_FRAMEBUFFER_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _FRAMEBUFFER FRAMEBUFFER, *PFRAMEBUFFER;
typedef const FRAMEBUFFER *PCFRAMEBUFFER;

//
// Functions
//

ISTATUS
FramebufferGetSize(
    _In_ PCFRAMEBUFFER framebuffer,
    _Out_ size_t *num_columns,
    _Out_ size_t *num_rows
    );

ISTATUS
FramebufferGetPixel(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ size_t column,
    _In_ size_t row,
    _Out_ PCOLOR3 color
    );

#endif // _IRIS_CAMERA_FRAMEBUFFER_