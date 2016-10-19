/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfallocator.h

Abstract:

    This file contains the definitions for the PHYSX_BRDF_ALLOCATOR type.

--*/

#ifndef _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxBrdfAllocatorCreate(
    _Out_ PPHYSX_BRDF_ALLOCATOR *Allocator
    );

VOID
PhysxBrdfAllocatorFreeAll(
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator
    );

VOID
PhysxBrdfAllocatorFree(
    _In_ _Post_invalid_ PPHYSX_BRDF_ALLOCATOR Allocator
    );

#endif // _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
