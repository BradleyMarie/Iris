/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant_emissive_material.h

Abstract:

    Creates an emissive material which returns a constant spectrum.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CONSTANT_EMISSIVE_MATERIAL_
#define _IRIS_PHYSX_TOOLKIT_CONSTANT_EMISSIVE_MATERIAL_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
ConstantEmissiveMaterialAllocate(
    _In_ PSPECTRUM spectrum,
    _Out_ PEMISSIVE_MATERIAL *material
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CONSTANT_EMISSIVE_MATERIAL_