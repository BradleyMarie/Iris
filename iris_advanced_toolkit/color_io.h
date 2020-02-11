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
ColorLoadFromFloats(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const float values[3],
    _Out_ PCOLOR3 color
    );

ISTATUS
ColorLoadFromBytes(
    _In_ COLOR_IO_FORMAT color_format,
    _In_ const unsigned char values[3],
    _Out_ PCOLOR3 color
    );

#endif // _IRIS_ADVANCED_TOOLKIT_COLOR_IO_