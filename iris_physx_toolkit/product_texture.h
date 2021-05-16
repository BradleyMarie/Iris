/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    product_texture.h

Abstract:

    Allocates a texture that returns the product of two textures.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PRODUCT_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_PRODUCT_TEXTURE_

#include "iris_physx_toolkit/float_texture.h"
#include "iris_physx_toolkit/reflector_texture.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ProductFloatTextureAllocate(
    _In_opt_ PFLOAT_TEXTURE multiplicand0,
    _In_opt_ PFLOAT_TEXTURE multiplicand1,
    _Out_ PFLOAT_TEXTURE *product
    );

ISTATUS
ProductReflectorTextureAllocate(
    _In_opt_ PREFLECTOR_TEXTURE multiplicand0,
    _In_opt_ PREFLECTOR_TEXTURE multiplicand1,
    _Out_ PREFLECTOR_TEXTURE *product
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PRODUCT_TEXTURE_