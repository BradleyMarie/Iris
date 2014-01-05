/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_types.h

Abstract:

    This module defines any shared internal iris types.

--*/

typedef
COMPARISON_RESULT
(*PLIST_SORT_ROUTINE)(
    _In_ PCVOID Element0,
    _In_ PCVOID Element1
    );