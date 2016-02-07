/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_utility.h

Abstract:

    This file contains the declarations for the 
    Iris++ utility routines.

--*/

#include <irisplusplus.h>

#ifndef _UTILITY_IRIS_PLUS_PLUS_
#define _UTILITY_IRIS_PLUS_PLUS_

namespace Iris {

//
// Functions
//

IRISPLUSPLUSAPI
const char *
ISTATUSToCString(
    _In_ ISTATUS Status
    );

} // namespace Iris

#endif // _UTILITY_IRIS_PLUS_PLUS_