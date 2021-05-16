/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    png_mipmap.c

Abstract:

    Creates a mipmap from a PNG file.

--*/

#include "iris_physx_toolkit/png_mipmap.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "iris_advanced_toolkit/lanczos_upscale.h"

//
// Functions
//

ISTATUS
PngReflectorMipmapAllocate(
    _In_z_ const char* filename,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    int x, y, n;
    unsigned char (*data)[3] =
        (unsigned char (*)[3])stbi_load(filename, &x, &y, &n, 3);

    if (data == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    PCOLOR3 colors;
    ISTATUS status = ColorLoadFromByteTupleArray(COLOR_IO_FORMAT_SRGB,
                                                 data,
                                                 x * y,
                                                 &colors);

    free(data);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t new_x, new_y;
    status = LanczosUpscaleColors(colors,
                                  (size_t)x,
                                  (size_t)y,
                                  &colors,
                                  &new_x,
                                  &new_y);

    if (status != ISTATUS_SUCCESS)
    {
        free(colors);
        return status;
    }

    status = ReflectorMipmapAllocate(colors,
                                     new_x,
                                     new_y,
                                     texture_filtering,
                                     max_anisotropy,
                                     wrap_mode,
                                     color_extrapolator,
                                     mipmap);

    free(colors);

    return status;
}

ISTATUS
PngFloatMipmapAllocate(
    _In_z_ const char* filename,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    int x, y, n;
    unsigned char (*data)[3] =
        (unsigned char (*)[3])stbi_load(filename, &x, &y, &n, 3);

    float_t *luma;
    ISTATUS status = ColorLoadLuminanceFromByteTupleArray(COLOR_IO_FORMAT_SRGB,
                                                          data,
                                                          x * y,
                                                          &luma);

    free(data);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t new_x, new_y;
    status = LanczosUpscaleFloats(luma,
                                  (size_t)x,
                                  (size_t)y,
                                  &luma,
                                  &new_x,
                                  &new_y);

    if (status != ISTATUS_SUCCESS)
    {
        free(luma);
        return status;
    }

    status = FloatMipmapAllocateFromFloats(luma,
                                           new_x,
                                           new_y,
                                           texture_filtering,
                                           max_anisotropy,
                                           wrap_mode,
                                           mipmap);

    free(luma);

    return status;
}