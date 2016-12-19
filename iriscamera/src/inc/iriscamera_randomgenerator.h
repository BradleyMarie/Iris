/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_rendomgenerator.h

Abstract:

    This file contains the internal definitions for the RANDOM_GENERATOR type.

--*/

#ifndef _RANDOM_GENERATOR_IRIS_CAMERA_INTERNAL_
#define _RANDOM_GENERATOR_IRIS_CAMERA_INTERNAL_

#include <iriscamera.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomGeneratorGenerateRandom(
    _In_ PCRANDOM_GENERATOR RngGenerator,
    _Out_ PRANDOM *Rng 
    );

#endif // _RANDOM_GENERATOR_IRIS_CAMERA_INTERNAL_