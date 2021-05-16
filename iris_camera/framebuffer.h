/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    framebuffer.h

Abstract:

    A 2D image comprised of XYZ color values for storing the output of the
    render routines. 
    
    On allocation, a framebuffer is initialized to contain only zero values.

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
FramebufferAllocate(
    _In_ size_t num_columns,
    _In_ size_t num_rows,
    _Out_ PFRAMEBUFFER *framebuffer
    );

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

void
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER framebuffer
    );

#endif // _IRIS_CAMERA_FRAMEBUFFER_