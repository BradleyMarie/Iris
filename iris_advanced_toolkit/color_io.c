/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_io.c

Abstract:

    Routines for converting colors into and out of storage formats.

--*/

#include "iris_advanced_toolkit/color_io.h"

#include "stdlib.h"

//
// Static Functions
//

float_t
SrgbToLinearSrgb(
    _In_ float_t value
    )
{
    if (value <= (float_t)0.04045)
    {
        return value / (float_t)12.92;
    }

    return pow((value + (float_t)0.055) / (float_t)1.055, (float_t)2.4);
}

//
// Functions
//

ISTATUS
ColorLoadFromFloatTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const float values[3],
    _Out_ PCOLOR3 color
    )
{
    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(values[0]) ||
        values[0] < (float_t)0.0 ||
        (float_t)1.0 < values[0])
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(values[1]) ||
        values[1] < (float_t)0.0 ||
        (float_t)1.0 < values[1])
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(values[2]) ||
        values[2] < (float_t)0.0 ||
        (float_t)1.0 < values[2])
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    float_t float_values[3];
    float_values[0] = SrgbToLinearSrgb((float_t)values[0]);
    float_values[1] = SrgbToLinearSrgb((float_t)values[1]);
    float_values[2] = SrgbToLinearSrgb((float_t)values[2]);

    *color = ColorCreate(COLOR_SPACE_LINEAR_SRGB, float_values);

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadFromByteTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const unsigned char values[3],
    _Out_ PCOLOR3 color
    )
{
    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    float_t float_values[3];
    float_values[0] = (float_t)values[0] / (float_t)255.0;
    float_values[1] = (float_t)values[1] / (float_t)255.0;
    float_values[2] = (float_t)values[2] / (float_t)255.0;

    return ColorLoadFromFloatTuple(color_format, float_values, color);
}

ISTATUS
ColorLoadLuminanceFromFloatTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const float values[3],
    _Out_ float_t *luma
    )
{
    COLOR3 color;
    ISTATUS status = ColorLoadFromFloatTuple(color_format,
                                             values,
                                             &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    color = ColorConvert(color, COLOR_SPACE_XYZ);
    color = ColorClamp(color, 1.0f);

    *luma = (float_t)color.values[1];

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadLuminanceFromByteTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const unsigned char values[3],
    _Out_ float_t *luma
    )
{
    COLOR3 color;
    ISTATUS status = ColorLoadFromByteTuple(color_format,
                                            values,
                                            &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    color = ColorConvert(color, COLOR_SPACE_XYZ);
    color = ColorClamp(color, 1.0f);

    *luma = (float_t)color.values[1];

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadFromFloatTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const float values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) PCOLOR3 *colors
    )
{
    if (colors == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (length == 0)
    {
        *colors = NULL;
        return ISTATUS_SUCCESS;
    }

    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCOLOR3 result = (PCOLOR3)calloc(length, sizeof(COLOR3));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < length; i++)
    {
        ISTATUS status = ColorLoadFromFloatTuple(color_format,
                                                 values[i],
                                                 result + i);

        if (status != ISTATUS_SUCCESS)
        {
            free(result);
            return status;
        }
    }

    *colors = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadFromByteTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const unsigned char values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) PCOLOR3 *colors
    )
{
    if (colors == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (length == 0)
    {
        *colors = NULL;
        return ISTATUS_SUCCESS;
    }

    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCOLOR3 result = (PCOLOR3)calloc(length, sizeof(COLOR3));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < length; i++)
    {
        ISTATUS status = ColorLoadFromByteTuple(color_format,
                                                values[i],
                                                result + i);

        if (status != ISTATUS_SUCCESS)
        {
            free(result);
            return status;
        }
    }

    *colors = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadLuminanceFromFloatTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const float values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) float_t **lumas
    )
{
    if (lumas == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (length == 0)
    {
        *lumas = NULL;
        return ISTATUS_SUCCESS;
    }

    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    float_t *result = (float_t *)calloc(length, sizeof(float_t));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < length; i++)
    {
        ISTATUS status = ColorLoadLuminanceFromFloatTuple(color_format,
                                                          values[i],
                                                          result + i);

        if (status != ISTATUS_SUCCESS)
        {
            free(result);
            return status;
        }
    }

    *lumas = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorLoadLuminanceFromByteTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const unsigned char values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) float_t **lumas
    )
{
    if (lumas == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (length == 0)
    {
        *lumas = NULL;
        return ISTATUS_SUCCESS;
    }

    if (values == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    float_t *result = (float_t *)calloc(length, sizeof(float_t));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < length; i++)
    {
        ISTATUS status = ColorLoadLuminanceFromByteTuple(color_format,
                                                         values[i],
                                                         result + i);

        if (status != ISTATUS_SUCCESS)
        {
            free(result);
            return status;
        }
    }

    *lumas = result;

    return ISTATUS_SUCCESS;
}