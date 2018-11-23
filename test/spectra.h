/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectra.h

Abstract:

    A spectrum, reflector, and tone mapper for use in testing which directly map
    X, Y, and Z values to the outputs of the integrator.

--*/

#ifndef _TEST_SPECTRA_
#define _TEST_SPECTRA_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
TestSpectrumAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PSPECTRUM *spectrum
    );

ISTATUS
TestReflectorAllocate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z,
    _Out_ PREFLECTOR *reflector
    );

ISTATUS
TestToneMapperAllocate(
    _Out_ PTONE_MAPPER *tone_mapper
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _TEST_SPECTRA_