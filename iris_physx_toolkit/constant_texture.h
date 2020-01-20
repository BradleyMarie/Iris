/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    constant_texture.h

Abstract:

    Allocates a texture that returns a constant value.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CONSTANT_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_CONSTANT_TEXTURE_

#include "iris_physx_toolkit/float_texture.h"
#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ConstantFloatTextureAllocate(
    _In_ float_t value,
    _Out_ PFLOAT_TEXTURE *texture
    );

ISTATUS
ConstantReflectorTextureAllocate(
    _In_opt_ PREFLECTOR reflector,
    _Out_ PREFLECTOR_TEXTURE *texture
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CONSTANT_TEXTURE_