/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iriscommon_types.h

Abstract:

    This module defines any shared internal iris types.

--*/

#ifndef _IRIS_COMMON_TYPES_
#define _IRIS_COMMON_TYPES_

#include <iris.h>

typedef
COMPARISON_RESULT
(*PLIST_SORT_ROUTINE)(
    _In_ PCVOID Element0,
    _In_ PCVOID Element1
    );

#endif // _IRIS_COMMON_TYPES_