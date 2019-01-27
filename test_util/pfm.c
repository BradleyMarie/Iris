/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    pfm.c

Abstract:

    Tests if a framebuffer matches a pfm file.

--*/

#include <assert.h>
#include <stdio.h>

#include "test_util/pfm.h"

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

static
void
SwapBytes(
    _Inout_updates_(size) void *bytes,
    _In_ size_t size
    )
{
    char* end = (char*)bytes + size - 1;
    char* begin = (char*)bytes;

    while (begin < end)
    {
        char temp = *begin;
        *begin = *end;
        *end = temp;

        begin++;
        end--;
    }
}

//
// Static Functions
//

ISTATUS
CheckEqualsPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format,
    _In_ float_t epsilon,
    _Out_ bool *result
    )
{
    assert(isfinite(epsilon));
    assert((float_t)0.0 <= epsilon);
    assert(result != NULL);

    if (framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (pixel_format > PFM_PIXEL_FORMAT_SRGB)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    if (fgetc(file) != 'P' ||
        fgetc(file) != 'F' ||
        fgetc(file) != '\n')
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    size_t num_columns, num_rows;
    if (fscanf(file, "%zu %zu", &num_columns, &num_rows) != 2)
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    size_t fb_num_columns, fb_num_rows;
    FramebufferGetSize(framebuffer, &fb_num_columns, &fb_num_rows);

    if (fb_num_rows != num_rows ||
        fb_num_columns != num_columns)
    {
        if (fclose(file) == EOF)
        {
            return ISTATUS_IO_ERROR;
        }

        *result = false;
        return ISTATUS_SUCCESS;
    }

    float endianness;
    if (fscanf(file, "%f", &endianness) != 1)
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    if (fgetc(file) != '\n')
    {
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    bool swap_bytes = IsLittleEndian() != (endianness < 0.0f);

    for (size_t i = 0; i < num_rows; i++)
    {
        for (size_t j = 0; j < num_columns; j++)
        {
            float x;
            if (fread(&x, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }

            if (swap_bytes)
            {
                SwapBytes(&x, sizeof(float));
            }

            float y;
            if (fread(&y, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }

            if (swap_bytes)
            {
                SwapBytes(&y, sizeof(float));
            }

            float z;
            if (fread(&z, sizeof(float), 1, file) != 1)
            {
                fclose(file);
                return ISTATUS_IO_ERROR;
            }

            if (swap_bytes)
            {
                SwapBytes(&z, sizeof(float));
            }

            COLOR3 pixel_color;
            FramebufferGetPixel(framebuffer, j, num_rows - i - 1, &pixel_color);

            if (fabs((float)pixel_color.x - x) > epsilon ||
                fabs((float)pixel_color.y - y) > epsilon ||
                fabs((float)pixel_color.z - z) > epsilon)
            {
                if (fclose(file) == EOF)
                {
                    return ISTATUS_IO_ERROR;
                }

                *result = false;
                return ISTATUS_SUCCESS;
            }
        }
    }

    if (fclose(file) == EOF)
    {
        return ISTATUS_IO_ERROR;
    }

    *result = true;
    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
ExactlyEqualsPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format,
    _Out_ bool *result
    )
{
    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = CheckEqualsPfmFile(framebuffer,
                                        filename,
                                        pixel_format,
                                        (float_t)0.0,
                                        result);

    return status;
}

ISTATUS
ApproximatelyEqualsPfmFile(
    _In_ PCFRAMEBUFFER framebuffer,
    _In_ const char* filename,
    _In_ PFM_PIXEL_FORMAT pixel_format,
    _In_ float_t epsilon,
    _Out_ bool *result
    )
{
    if (!isfinite(epsilon) || (float_t)0.0 > epsilon)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = CheckEqualsPfmFile(framebuffer,
                                        filename,
                                        pixel_format,
                                        epsilon,
                                        result);

    return status;
}