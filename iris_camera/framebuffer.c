/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    framebuffer.h

Abstract:

    A 2D image of XYZ values.

--*/

#include <stdlib.h>

#include "common/safe_math.h"
#include "iris_camera/framebuffer.h"
#include "iris_camera/framebuffer_internal.h"

//
// Defines
//

#define FRAMEBUFFER_DATA_ALIGNMENT 128

//
// Functions
//

ISTATUS
FramebufferAllocate(
    _In_ size_t num_columns,
    _In_ size_t num_rows,
    _Out_ PFRAMEBUFFER *framebuffer
    )
{
    if (num_columns == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (num_rows == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(num_columns,
                                        num_rows,
                                        &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t num_bytes;
    success = CheckedMultiplySizeT(num_pixels,
                                   sizeof(COLOR3),
                                   &num_bytes);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PFRAMEBUFFER result = (PFRAMEBUFFER)malloc(sizeof(FRAMEBUFFER));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->data =
        (PCOLOR3)aligned_alloc(FRAMEBUFFER_DATA_ALIGNMENT, num_bytes);

    if (result->data == NULL)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->num_columns = num_columns;
    result->num_rows = num_rows;

    *framebuffer = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FramebufferGetSize(
    _In_ PCFRAMEBUFFER framebuffer,
    _Out_ size_t *num_columns,
    _Out_ size_t *num_rows
    )
{
    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_columns == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (num_rows == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    *num_columns = framebuffer->num_columns;
    *num_rows = framebuffer->num_rows;

    return ISTATUS_SUCCESS;
}

ISTATUS
FramebufferGetPixel(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ size_t column,
    _In_ size_t row,
    _Out_ PCOLOR3 color
    )
{
    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (framebuffer->num_columns <= column)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (framebuffer->num_rows <= row)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    *color = framebuffer->data[framebuffer->num_columns * row + column];

    return ISTATUS_SUCCESS;
}

void
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER framebuffer
    )
{
    if (framebuffer == NULL)
    {
        return;
    }

    free(framebuffer->data);
    free(framebuffer);
}