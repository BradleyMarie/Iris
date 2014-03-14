/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_constants.h

Abstract:

    This file contains the functions to verify the usage of the Iris Toolkit
    constants.

--*/

#ifndef _CONSTANTS_IRIS_TOOLKIT_INTERNAL_
#define _CONSTANTS_IRIS_TOOLKIT_INTERNAL_

#include <iristoolkitp.h>

#define IRIS_TOOLKIT_NORMAL_IS_PRENORMALIZED_MASK   0xFD
#define IRIS_TOOLKIT_NORMAL_IS_MODEL_MASK           0xFE

SFORCEINLINE
BOOL
VerifyNormalType(
    UINT8 Type
    )
{
    return (Type <= IRIS_TOOLKIT_NORMAL_MODEL_PRENORMALIZED) ? TRUE : FALSE;
}

SFORCEINLINE
BOOL
IsPrenormalizedNormal(
    UINT8 Type
    )
{
    return ((Type & IRIS_TOOLKIT_NORMAL_IS_PRENORMALIZED_MASK) != 0) ? TRUE : FALSE;
}

SFORCEINLINE
BOOL
IsModelNormal(
    UINT8 Type
    )
{
    return ((Type & IRIS_TOOLKIT_NORMAL_IS_MODEL_MASK) != 0) ? TRUE : FALSE;
}

#endif // _CONSTANTS_IRIS_TOOLKIT_INTERNAL_