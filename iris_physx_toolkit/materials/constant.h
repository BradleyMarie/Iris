/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant.h

Abstract:

    Creates a material that does not support normal mapping which always returns
    a single BSDF.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MATERIALS_CONSTANT_
#define _IRIS_PHYSX_TOOLKIT_MATERIALS_CONSTANT_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ConstantMaterialAllocate(
    _In_opt_ PBSDF bsdf,
    _Out_ PMATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MATERIALS_CONSTANT_