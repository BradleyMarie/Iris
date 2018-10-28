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
    uint8_t byte_array[] = { 255, 0 };
    return *((uint16_t*)byte_array) == 255;
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

    if (pixel_format != PFM_PIXEL_FORMAT_XYZ)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
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
            FramebufferGetPixel(framebuffer, j, i, &pixel_color);

            float x = (float)pixel_color.x;
            float y = (float)pixel_color.y;
            float z = (float)pixel_color.z;

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