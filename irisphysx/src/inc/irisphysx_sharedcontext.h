/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_sharedcontext.h

Abstract:

    This file contains the internal definitions for the 
    PBR_SHARED_CONTEXT type.

--*/

#ifndef _PBR_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_
#define _PBR_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PBR_SHARED_CONTEXT {
    PBR_VISIBILITY_TESTER PBRVisibilityTester;
    PBR_BRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    _Field_size_opt_(NumberOfLights) PCPBR_LIGHT *Lights;
    SIZE_T NumberOfLights;
    PRANDOM Rng;
    FLOAT Epsilon;
} PBR_SHARED_CONTEXT, *PPBR_SHARED_CONTEXT;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRSharedContextInitialize(
    _Out_ PPBR_SHARED_CONTEXT PBRSharedContext
    )
{   
    ISTATUS Status;
     
    ASSERT(PBRSharedContext != NULL);
    
    Status = PBRVisibilityTesterInitialize(&PBRSharedContext->PBRVisibilityTester);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    PbrBrdfAllocatorInitialize(&PBRSharedContext->BrdfAllocator);
    
    Status = SpectrumCompositorAllocate(&PBRSharedContext->SpectrumCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        PbrBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
        PBRVisibilityTesterDestroy(&PBRSharedContext->PBRVisibilityTester);
        return Status;
    }
    
    Status = ReflectorCompositorAllocate(&PBRSharedContext->ReflectorCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumCompositorFree(PBRSharedContext->SpectrumCompositor);
        PbrBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
        PBRVisibilityTesterDestroy(&PBRSharedContext->PBRVisibilityTester);
        return Status;
    }
    
    PBRSharedContext->TestGeometryRoutine = NULL;
    PBRSharedContext->TestGeometryRoutineContext = NULL;
    PBRSharedContext->Lights = NULL;
    PBRSharedContext->NumberOfLights = 0;
    PBRSharedContext->Rng = NULL;
    PBRSharedContext->Epsilon = (FLOAT) 0.0f;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRSharedContextSet(
    _Inout_ PPBR_SHARED_CONTEXT PBRSharedContext,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_reads_(NumberOfLights) PCPBR_LIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon
    )
{    
    ASSERT(PBRSharedContext != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT((Lights == NULL && NumberOfLights == 0) || (Lights != NULL && NumberOfLights != 0));
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));
    
    PBRSharedContext->TestGeometryRoutine = TestGeometryRoutine;
    PBRSharedContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
    PBRSharedContext->Lights = Lights;
    PBRSharedContext->NumberOfLights = NumberOfLights;
    PBRSharedContext->Rng = Rng;
    PBRSharedContext->Epsilon = Epsilon;   
}

SFORCEINLINE
VOID
PBRSharedContextDestroy(
    _In_opt_ _Post_invalid_ PPBR_SHARED_CONTEXT PBRSharedContext
    )
{
    ReflectorCompositorFree(PBRSharedContext->ReflectorCompositor);
    SpectrumCompositorFree(PBRSharedContext->SpectrumCompositor);
    PbrBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
    PBRVisibilityTesterDestroy(&PBRSharedContext->PBRVisibilityTester);
}

#endif // _PBR_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_