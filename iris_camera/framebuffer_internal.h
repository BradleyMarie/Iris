/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    framebuffer_internal.h

Abstract:

    The internal routines for a framebuffer.

--*/

#ifndef _IRIS_CAMERA_FRAMEBUFFER_INTERNAL_
#define _IRIS_CAMERA_FRAMEBUFFER_INTERNAL_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

struct _FRAMEBUFFER {
    _Field_size_(num_columns * num_rows) PCOLOR3 data;
    size_t num_columns;
    size_t num_rows;
};

//
// Functions
//

static
inline
void
FramebufferSetPixel(
    _Inout_ struct _FRAMEBUFFER *framebuffer,
    _In_ size_t column,
    _In_ size_t row,
    _In_ COLOR3 color
    )
{
    assert(framebuffer != NULL);
    assert(column < framebuffer->num_columns);
    assert(row < framebuffer->num_rows);
    assert(ColorValidate(color));

    framebuffer->data[framebuffer->num_columns * row + column] = color;
}

#endif // _IRIS_CAMERA_FRAMEBUFFER_INTERNAL_