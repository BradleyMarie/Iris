/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisrandomtoolkit_mwc.h

Abstract:

    This file contains the prototypes for the MWC_RNG type.

--*/

#ifndef _MULTIPLY_WITH_CARRY_IRIS_RANDOM_TOOLKIT_
#define _MULTIPLY_WITH_CARRY_IRIS_RANDOM_TOOLKIT_

#include <irisrandomtoolkit.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRANDOMTOOLKITAPI
ISTATUS
MultiplyWithCarryRngAllocate(
    _Out_ PRANDOM *Rng
    );

#endif // _MULTIPLY_WITH_CARRY_IRIS_RANDOM_TOOLKIT_