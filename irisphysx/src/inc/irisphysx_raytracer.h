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

struct _PBR_RAYTRACER {
    PPBR_RAYTRACER NextPRBRayTracer;
    PPBR_SHARED_CONTEXT SharedContext;
    PRAYTRACER RayTracer;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRRayTracerAllocate(
    _In_opt_ PPBR_RAYTRACER NextPRBRayTracer,
    _In_ PPBR_SHARED_CONTEXT SharedContext,
    _Out_ PPBR_RAYTRACER *Result
    )
{
    PPBR_RAYTRACER PBRRayTracer;
    PVOID Allocation;
    ISTATUS Status;
    
    ASSERT(Result != NULL);
    
    Allocation = malloc(sizeof(PBR_RAYTRACER));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    PBRRayTracer = (PPBR_RAYTRACER) Allocation;
    
    Status = RayTracerAllocate(&PBRRayTracer->RayTracer);
    
    if (Status != ISTATUS_SUCCESS)
    {
        free(Allocation);
        return Status;
    }
    
    PBRRayTracer->SharedContext = SharedContext;
    PBRRayTracer->NextPRBRayTracer = NextPRBRayTracer;
    
    *Result = PBRRayTracer;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRRayTracerFree(
    _In_opt_ _Post_invalid_ PPBR_RAYTRACER PBRRayTracer
    )
{
    if (PBRRayTracer == NULL)
    {
        return;
    }
    
    PBRRayTracerFree(PBRRayTracer->NextPRBRayTracer);
    RayTracerFree(PBRRayTracer->RayTracer);
    free(PBRRayTracer);
}

#endif // _RAYTRACER_IRIS_PHYSX_INTERNAL_