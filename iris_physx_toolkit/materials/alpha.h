/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    alpha.h

Abstract:

    Creates a material using an existing material for use with alpha blending.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MATERIALS_ALPHA_
#define _IRIS_PHYSX_TOOLKIT_MATERIALS_ALPHA_

#include "iris_physx_toolkit/float_texture.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
AlphaMaterialAllocate(
    _In_ PMATERIAL base_material,
    _In_ PFLOAT_TEXTURE alpha_texture,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MATERIALS_ALPHA_