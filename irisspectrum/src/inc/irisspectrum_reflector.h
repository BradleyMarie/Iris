/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_reflector.h

Abstract:

    This file contains the internal definitions for the REFLECTOR type.

--*/

#ifndef _REFLECTOR_IRIS_SPECTRUM_INTERNAL_
#define _REFLECTOR_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Types
//

struct _REFLECTOR {
    PCREFLECTOR_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

#endif // _REFLECTOR_IRIS_SPECTRUM_INTERNAL_