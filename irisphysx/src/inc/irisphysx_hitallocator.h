/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This file contains the definitions for the PHYSX_HIT_ALLOCATOR type.

--*/

#ifndef _PHYSX_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PHYSX_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_HIT_ALLOCATOR {
    PHIT_ALLOCATOR Allocator;
    PCPHYSX_GEOMETRY Geometry;
    RAY TraceRay;
};

//
// Functions
//

SFORCEINLINE
VOID
PhysxHitAllocatorInitialize(
    _Out_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _In_ PHIT_ALLOCATOR Allocator,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ RAY TraceRay
    )
{
    ASSERT(HitAllocator != NULL);
    ASSERT(Allocator != NULL);
    ASSERT(Geometry != NULL);
    ASSERT(RayValidate(TraceRay) != FALSE);
    
    HitAllocator->Allocator = Allocator;
    HitAllocator->Geometry = Geometry;
    HitAllocator->TraceRay = TraceRay;
}

SFORCEINLINE
VOID
PhysxHitAllocatorSetGeometry(
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ASSERT(HitAllocator != NULL);
    ASSERT(Geometry != NULL);
    
    HitAllocator->Geometry = Geometry;
}

SFORCEINLINE
PCPHYSX_GEOMETRY
PhysxHitAllocatorGetGeometry(
    _In_ PCPHYSX_HIT_ALLOCATOR HitAllocator
    )
{
    ASSERT(HitAllocator != NULL);
    
    return HitAllocator->Geometry;
}

SFORCEINLINE
RAY
PhysxHitAllocatorGetRay(
    _In_ PCPHYSX_HIT_ALLOCATOR HitAllocator
    )
{
    ASSERT(HitAllocator != NULL);
    
    return HitAllocator->TraceRay;
}

#endif // _PHYSX_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_
