/*++

Copyright (c) 2018 Brad Weinberger

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
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format
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

            float x, y, z;
            if (pixel_format == PFM_PIXEL_FORMAT_XYZ)
            {
                x = (float)pixel_color.x;
                y = (float)pixel_color.y;
                z = (float)pixel_color.z;
            }
            else // pixel_format == PFM_PIXEL_FORMAT_SRGB
            {
                x = 3.2404542f * (float)pixel_color.x -
                    1.5371385f * (float)pixel_color.y -
                    0.4985314f * (float)pixel_color.z;

                y = -0.969266f * (float)pixel_color.x +
                    1.8760108f * (float)pixel_color.y +
                    0.0415560f * (float)pixel_color.z;

                z = 0.0556434f * (float)pixel_color.x -
                    0.2040259f * (float)pixel_color.y +
                    1.0572252f * (float)pixel_color.z;
            }

            if (fwrite(&x, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }

            if (fwrite(&y, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }

            if (fwrite(&z, sizeof(float), 1, file) != 1)
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