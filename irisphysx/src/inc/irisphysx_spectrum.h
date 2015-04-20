/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_spectrum.h

Abstract:

    This file contains the internal definitions for the SPECTRUM type.

--*/

#ifndef _SPECTRUM_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM {
    PCSPECTRUM_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

#endif // _SPECTRUM_IRIS_PHYSX_INTERNAL_