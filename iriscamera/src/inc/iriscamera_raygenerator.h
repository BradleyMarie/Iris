/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_raygenerator.h

Abstract:

    This file contains the internal definitions for the RAY_GENERATOR type.

--*/

#ifndef _RAY_GENERATOR_IRIS_CAMERA_INTERNAL_
#define _RAY_GENERATOR_IRIS_CAMERA_INTERNAL_

#include <iriscamerap.h>

//
// Types
//

struct _RAY_GENERATOR {
    PCCAMERA Camera;
    SIZE_T Row;
    SIZE_T NumberOfRows;
    SIZE_T Column;
    SIZE_T NumberOfColumns;
};

//
// Functions
//

SFORCEINLINE
RAY_GENERATOR
RayGeneratorCreate(
    _In_ PCCAMERA Camera,
    _In_range_(0, NumberOfRows - 1) SIZE_T Row,
    _In_ SIZE_T NumberOfRows,
    _In_range_(0, NumberOfColumns - 1) SIZE_T Column,
    _In_ SIZE_T NumberOfColumns
    )
{
    RAY_GENERATOR Result;

    ASSERT(Camera != NULL);
    ASSERT(Row < NumberOfRows);
    ASSERT(Column < NumberOfColumns);

    Result.Camera = Camera;
    Result.Row = Row;
    Result.NumberOfRows = NumberOfRows;
    Result.Column = Column;
    Result.NumberOfColumns = NumberOfColumns;

    return Result;
}

#endif // _RAY_GENERATOR_IRIS_CAMERA_INTERNAL_