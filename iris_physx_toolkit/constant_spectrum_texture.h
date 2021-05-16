/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant_spectrum_texture.h

Abstract:

    Allocates a spectrum texture that returns a constant value.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CONSTANT_SPECTRUM_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_CONSTANT_SPECTRUM_TEXTURE_

#include "iris_physx_toolkit/spectrum_texture.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ConstantSpectrumTextureAllocate(
    _In_opt_ PSPECTRUM spectrum,
    _Out_ PSPECTRUM_TEXTURE *texture
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CONSTANT_SPECTRUM_TEXTURE_