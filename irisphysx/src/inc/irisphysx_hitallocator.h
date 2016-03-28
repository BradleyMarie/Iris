/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This file contains the definitions for the PBR_HIT_ALLOCATOR type.

--*/

#ifndef _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PBR_HIT_ALLOCATOR {
    PHIT_ALLOCATOR Allocator;
    PCPBR_GEOMETRY Geometry;
    RAY TraceRay;
};

//
// Functions
//

SFORCEINLINE
VOID
PBRHitAllocatorInitialize(
    _Out_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _In_ PHIT_ALLOCATOR Allocator,
    _In_ PCPBR_GEOMETRY Geometry,
    _In_ RAY TraceRay
    )
{
    ASSERT(PBRHitAllocator != NULL);
    ASSERT(Allocator != NULL);
    ASSERT(Geometry != NULL);
    ASSERT(RayValidate(TraceRay) != FALSE);
    
    PBRHitAllocator->Allocator = Allocator;
    PBRHitAllocator->Geometry = Geometry;
    PBRHitAllocator->TraceRay = TraceRay;
}

SFORCEINLINE
VOID
PBRHitAllocatorSetGeometry(
    _Inout_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _In_ PCPBR_GEOMETRY PBRGeometry
    )
{
    ASSERT(PBRHitAllocator != NULL);
    ASSERT(PBRGeometry != NULL);
    
    PBRHitAllocator->Geometry = PBRGeometry;
}

SFORCEINLINE
PCPBR_GEOMETRY
PBRHitAllocatorGetGeometry(
    _In_ PCPBR_HIT_ALLOCATOR PBRHitAllocator
    )
{
    ASSERT(PBRHitAllocator != NULL);
    
    return PBRHitAllocator->Geometry;
}

SFORCEINLINE
RAY
PBRHitAllocatorGetRay(
    _In_ PCPBR_HIT_ALLOCATOR PBRHitAllocator
    )
{
    ASSERT(PBRHitAllocator != NULL);
    
    return PBRHitAllocator->TraceRay;
}

#endif // _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_