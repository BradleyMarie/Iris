/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    framebuffer.c

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

#define FRAMEBUFFER_ROW_ALIGNMENT 128

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

    size_t row_size;
    bool success = CheckedMultiplySizeT(num_columns,
                                        sizeof(COLOR3),
                                        &row_size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    size_t byte_over = row_size % FRAMEBUFFER_ROW_ALIGNMENT;
    if (byte_over != 0)
    {
        size_t to_add = FRAMEBUFFER_ROW_ALIGNMENT - byte_over;
        success = CheckedAddSizeT(row_size,
                                  to_add,
                                  &row_size);

        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    size_t num_bytes;
    success = CheckedMultiplySizeT(row_size,
                                   num_rows,
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
        (char *)aligned_alloc(FRAMEBUFFER_ROW_ALIGNMENT, num_bytes);

    if (result->data == NULL)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->row_size = row_size;
    result->num_columns = num_columns;
    result->num_rows = num_rows;

    for (size_t i = 0; i < num_rows; i++)
    {
        PCOLOR3 row = (void *)(result->data + i * row_size);
        for (size_t j = 0; j < num_columns; j++)
        {
            row[j] = ColorCreateBlack();
        }
    }

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

    PCCOLOR3 row_data =
        (const void *)(framebuffer->data + row * framebuffer->row_size);
    *color = row_data[column];

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