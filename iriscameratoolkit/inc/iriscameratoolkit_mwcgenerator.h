/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkit_mwc.h

Abstract:

    This file contains the prototypes for the MWC_RNG_GENERATOR type.

--*/

#ifndef _MWC_RNG_GENERATOR_IRIS_CAMERA_TOOLKIT_
#define _MWC_RNG_GENERATOR_IRIS_CAMERA_TOOLKIT_

#include <iriscameratoolkit.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERATOOLKITAPI
ISTATUS
MultiplyWithCarryGeneratorAllocate(
    _Out_ PRANDOM_GENERATOR *RngGenerator
    );

#endif // _MWC_RNG_GENERATOR_IRIS_CAMERA_TOOLKIT_