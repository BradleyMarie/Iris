/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the prototypes for the RayTracer functions.

--*/

#ifndef _RAYTRACER_IRIS_PHYSX_INTERNAL_
#define _RAYTRACER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_RAYTRACER {
    PPHYSX_RAYTRACER NextRayTracer;
    PPHYSX_SHARED_CONTEXT SharedContext;
    PRAYTRACER RayTracer;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxRayTracerAllocate(
    _In_opt_ PPHYSX_RAYTRACER NextRayTracer,
    _In_ PPHYSX_SHARED_CONTEXT SharedContext,
    _Out_ PPHYSX_RAYTRACER *Result
    )
{
    PPHYSX_RAYTRACER RayTracer;
    PVOID Allocation;
    ISTATUS Status;
    
    ASSERT(Result != NULL);
    
    Allocation = malloc(sizeof(PHYSX_RAYTRACER));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    RayTracer = (PPHYSX_RAYTRACER) Allocation;
    
    Status = RayTracerAllocate(&RayTracer->RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        free(Allocation);
        return Status;
    }
    
    RayTracer->SharedContext = SharedContext;
    RayTracer->NextRayTracer = NextRayTracer;
    
    *Result = RayTracer;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PhysxRayTracerFree(
    _In_opt_ _Post_invalid_ PPHYSX_RAYTRACER RayTracer
    )
{
    if (RayTracer == NULL)
    {
        return;
    }
    
    PhysxRayTracerFree(RayTracer->NextRayTracer);
    RayTracerFree(RayTracer->RayTracer);
    free(RayTracer);
}

#endif // _RAYTRACER_IRIS_PHYSX_INTERNAL_
