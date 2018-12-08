/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    attenuated_reflector.h

Abstract:

    Creates a reflector from one or more attenuated reflectors. The sum of the
    attenuations must be greater than or equal to zero and less than or equal
    to one.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_ATTENUATED_REFLECTOR_
#define _IRIS_PHYSX_TOOLKIT_ATTENUATED_REFLECTOR_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
AttenuatedReflector3AllocateWithAllocator(
    _In_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _In_opt_ PREFLECTOR reflector0,
    _In_ float_t attenuation0,
    _In_opt_ PREFLECTOR reflector1,
    _In_ float_t attenuation1,
    _In_opt_ PREFLECTOR reflector2,
    _In_ float_t attenuation2,
    _Out_ PCREFLECTOR *reflector
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_ATTENUATED_REFLECTOR_