/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    uniform_reflector.h

Abstract:

    Creates a reflector which reflects light uniformly across all wavelengths.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_UNIFORM_REFLECTOR_
#define _IRIS_PHYSX_TOOLKIT_UNIFORM_REFLECTOR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
UniformReflectorAllocate(
    _In_ float_t albedo,
    _Out_ PREFLECTOR *reflector
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_UNIFORM_REFLECTOR_