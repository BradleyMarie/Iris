/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    png_mipmap.c

Abstract:

    Creates a mipmap from a PNG file.

--*/

#include "iris_physx_toolkit/png_mipmap.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "third_party/stb/stb_image.h"

//
// Functions
//

ISTATUS
PngReflectorMipmapAllocate(
    _In_z_ const char* filename,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    int x, y, n;
    unsigned char (*data)[3] =
        (unsigned char (*)[3])stbi_load(filename, &x, &y, &n, 3);

    if (data == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    ISTATUS status = ReflectorMipmapAllocateFromBytes(COLOR_IO_FORMAT_SRGB,
                                                      data,
                                                      x,
                                                      y,
                                                      wrap_mode,
                                                      color_extrapolator,
                                                      mipmap);

    free(data);

    return status;
}

ISTATUS
PngFloatMipmapAllocate(
    _In_z_ const char* filename,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    int x, y, n;
    unsigned char (*data)[3] =
        (unsigned char (*)[3])stbi_load(filename, &x, &y, &n, 3);

    if (data == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    ISTATUS status = FloatMipmapAllocateFromByteTuples(COLOR_IO_FORMAT_SRGB,
                                                       data,
                                                       x,
                                                       y,
                                                       wrap_mode,
                                                       mipmap);

    free(data);

    return status;
}