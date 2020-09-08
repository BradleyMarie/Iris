/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_io.h

Abstract:

    Routines for converting colors into and out of storage formats.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_COLOR_IO_
#define _IRIS_ADVANCED_TOOLKIT_COLOR_IO_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef enum _COLOR_IO_FORMAT {
    COLOR_IO_FORMAT_SRGB = 0,
} COLOR_IO_FORMAT, *PCOLOR_IO_FORMAT;

typedef const COLOR_IO_FORMAT *PCCOLOR_IO_FORMAT;

//
// Functions
//

ISTATUS
ColorLoadFromFloatTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const float values[3],
    _Out_ PCOLOR3 color
    );

ISTATUS
ColorLoadFromByteTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const unsigned char values[3],
    _Out_ PCOLOR3 color
    );

ISTATUS
ColorLoadLuminanceFromFloatTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const float values[3],
    _Out_ float_t *luma
    );

ISTATUS
ColorLoadLuminanceFromByteTuple(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const unsigned char values[3],
    _Out_ float_t *luma
    );

ISTATUS
ColorLoadFromFloatTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const float values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) PCOLOR3 *colors
    );

ISTATUS
ColorLoadFromByteTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const unsigned char values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) PCOLOR3 *colors
    );

ISTATUS
ColorLoadLuminanceFromFloatTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const float values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) float_t **lumas
    );

ISTATUS
ColorLoadLuminanceFromByteTupleArray(
    _In_ COLOR_IO_FORMAT color_format,
    _In_reads_(length) const unsigned char values[][3],
    _In_ size_t length,
    _Outptr_result_buffer_(length) float_t **lumas
    );

#endif // _IRIS_ADVANCED_TOOLKIT_COLOR_IO_