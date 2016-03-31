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

typedef struct _PBR_RAYTRACER_SHARED_CONTEXT {
    PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    _Field_size_opt_(NumberOfLights) PCLIGHT *Lights;
    SIZE_T NumberOfLights;
    PPBR_VISIBILITY_TESTER PBRVisibilityTester;
    PBRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PRANDOM Rng;
    FLOAT Epsilon;
} PBR_RAYTRACER_SHARED_CONTEXT, *PPBR_RAYTRACER_SHARED_CONTEXT;

struct _PBR_RAYTRACER {
    PPBR_RAYTRACER NextPRBRayTracer;
    PPBR_RAYTRACER_SHARED_CONTEXT SharedContext;
    PRAYTRACER RayTracer;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRRayTracerSharedContextAllocate(
    _Out_ PPBR_RAYTRACER_SHARED_CONTEXT *Result
    )
{
    PPBR_RAYTRACER_SHARED_CONTEXT SharedContext;
    PVOID Allocation;
    
    ASSERT(SharedContext != NULL);
    
    Allocation = malloc(sizeof(PBR_RAYTRACER_SHARED_CONTEXT));
    
    if (Allocation = NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    SharedContext = (PPBR_RAYTRACER_SHARED_CONTEXT) Allocation;
    
    SharedContext->TestGeometryRoutine = NULL;
    SharedContext->TestGeometryRoutineContext = NULL;
    SharedContext->Lights = NULL;
    SharedContext->NumberOfLights = 0;
    SharedContext->PBRVisibilityTester = NULL;
    SharedContext->SpectrumCompositor = NULL;
    SharedContext->ReflectorCompositor = NULL;
    SharedContext->Rng = NULL;
    SharedContext->Epsilon = (FLOAT) 0.0f;
    
    *Result = SharedContext;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRRayTracerSharedContextFree(
    _In_opt_ _Post_invalid_ PPBR_RAYTRACER_SHARED_CONTEXT PBRRayTracerSharedContext
    )
{
    free(PBRRayTracerSharedContext);    
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRRayTracerAllocate(
    _In_opt_ PPBR_RAYTRACER NextPRBRayTracer,
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
    
    if (NextPRBRayTracer != NULL)
    {
        Status = PBRRayTracerSharedContextAllocate(&PBRRayTracer->SharedContext);
        
        if (Status != ISTATUS_SUCCESS)
        {
            RayTracerFree(PBRRayTracer->RayTracer);
            free(Allocation);
            return Status;
        }
        
        PBRRayTracer->NextPRBRayTracer = NULL;
    }
    else
    {
        PBRRayTracer->SharedContext = NextPRBRayTracer->SharedContext;
        PBRRayTracer->NextPRBRayTracer = NextPRBRayTracer;
    }
    
    *Result = PBRRayTracer;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRRayTracerSetSharedContextAllocators(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ PBRDF_ALLOCATOR BrdfAllocator,
    _In_ PSPECTRUM_COMPOSITOR SpectrumCompositor,
    _In_ PREFLECTOR_COMPOSITOR ReflectorCompositor
    )
{
    PPBR_RAYTRACER_SHARED_CONTEXT SharedContext;
    
    ASSERT(PBRRayTracer != NULL);
    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(BrdfAllocator != NULL);
    ASSERT(SpectrumCompositor != NULL);
    ASSERT(ReflectorCompositor != NULL);
    
    SharedContext = PBRRayTracer->SharedContext;
    
    SharedContext->PBRVisibilityTester = PBRVisibilityTester;
    SharedContext->BrdfAllocator = BrdfAllocator;
    SharedContext->SpectrumCompositor = SpectrumCompositor;
    SharedContext->ReflectorCompositor = ReflectorCompositor;
}

SFORCEINLINE
VOID
PBRRayTracerSetSharedContext(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_reads_(NumberOfLights) PCLIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon
    )
{
    PPBR_RAYTRACER_SHARED_CONTEXT SharedContext;
    
    ASSERT(PBRRayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT((Lights == NULL && NumberOfLights == 0) || (Lights != NULL && NumberOfLights != 0));
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));
    
    SharedContext = PBRRayTracer->SharedContext;

    SharedContext->TestGeometryRoutine = TestGeometryRoutine;
    SharedContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
    SharedContext->Lights = Lights;
    SharedContext->NumberOfLights = NumberOfLights;
    SharedContext->Rng = Rng;
    SharedContext->Epsilon = Epsilon;   
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
    
    if (PBRRayTracer->NextPRBRayTracer == NULL)
    {
        PBRRayTracerSharedContextFree(PBRRayTracer->SharedContext);
    }
    else
    {
        PBRRayTracerFree(PBRRayTracer->NextPRBRayTracer);
    }
    
    RayTracerFree(PBRRayTracer->RayTracer);
    free(PBRRayTracer);
}

#endif // _RAYTRACER_IRIS_PHYSX_INTERNAL_