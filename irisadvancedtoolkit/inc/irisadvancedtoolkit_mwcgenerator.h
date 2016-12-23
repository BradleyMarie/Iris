/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkit_mwc.h

Abstract:

    This file contains the prototypes for the MWC_RNG type.

--*/

#ifndef _MULTIPLY_WITH_CARRY_IRIS_ADVANCED_TOOLKIT_
#define _MULTIPLY_WITH_CARRY_IRIS_ADVANCED_TOOLKIT_

#include <irisadvancedtoolkit.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDTOOLKITAPI
ISTATUS
MultiplyWithCarryRandomGeneratorAllocate(
    _Out_ PRANDOM_GENERATOR *RandomGenerator
    );

#endif // _MULTIPLY_WITH_CARRY_IRIS_ADVANCED_TOOLKIT_