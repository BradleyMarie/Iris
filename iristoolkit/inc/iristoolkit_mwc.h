/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_mwc.h

Abstract:

    This file contains the prototypes for the MWC_RNG type.

--*/

#ifndef _MULTIPLY_WITH_CARRY_IRIS_TOOLKIT_
#define _MULTIPLY_WITH_CARRY_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PRANDOM
MultiplyWithCarryRngAllocate(
    VOID
    );

#endif // _MULTIPLY_WITH_CARRY_IRIS_TOOLKIT_