/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    pfm_writer.c

Abstract:

    Writes a framebuffer out to a pfm file.

--*/

#include <stdio.h>

#include "iris_camera_toolkit/pfm_writer.h"

//
// Static Functions
//

static
bool 
IsLittleEndian(
    void
    )
{
    union {
        uint16_t value;
        char bytes[2];
    } value = { 0x0001 };

    return value.bytes[0] == 1;
}

//
// Functions
//

ISTATUS
WriteToPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_z_ const char* filename,
    _In_ COLOR_SPACE color_space
    )
{
    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    size_t num_columns, num_rows;
    FramebufferGetSize(framebuffer, &num_columns, &num_rows);

    if (fprintf(file, "PF\n") < 0)
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    if (fprintf(file, "%zu %zu\n", num_columns, num_rows) < 0)
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    if (fprintf(file, "%lf\n", IsLittleEndian() ? -1.0f : 1.0f) < 0)
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    for (size_t i = 0; i < num_rows; i++)
    {
        for (size_t j = 0; j < num_columns; j++)
        {
            COLOR3 pixel_color;
            FramebufferGetPixel(framebuffer, j, num_rows - i - 1, &pixel_color);

            pixel_color = ColorConvert(pixel_color, color_space);

            if (fwrite(pixel_color.values, sizeof(float), 3, file) != 3)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }
        }
    }

    if (fclose(file) == EOF)
    {
        return ISTATUS_IO_ERROR;
    }

    return ISTATUS_SUCCESS;
}