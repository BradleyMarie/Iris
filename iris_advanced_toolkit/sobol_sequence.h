/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    sobol_sequence.h

Abstract:

    Creates a Sobol sequence.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_SOBOL_SEQUENCE_
#define _IRIS_ADVANCED_TOOLKIT_SOBOL_SEQUENCE_

#include "iris_advanced_toolkit/low_discrepancy_sequence.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
SobolSequenceAllocate(
    _Out_ PLOW_DISCREPANCY_SEQUENCE *sequence
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_ADVANCED_TOOLKIT_SOBOL_SEQUENCE_