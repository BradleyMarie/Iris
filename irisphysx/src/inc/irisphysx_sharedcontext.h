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
    PHYSX_VISIBILITY_TESTER VisibilityTester;
    PHYSX_BRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    PCPHYSX_LIGHT_LIST LightList;
    PRANDOM_REFERENCE Rng;
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
    
    Status = PhysxVisibilityTesterInitialize(&PBRSharedContext->VisibilityTester);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    PhysxBrdfAllocatorInitialize(&PBRSharedContext->BrdfAllocator);
    
    Status = SpectrumCompositorAllocate(&PBRSharedContext->SpectrumCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        PhysxBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
        PhysxVisibilityTesterDestroy(&PBRSharedContext->VisibilityTester);
        return Status;
    }
    
    Status = ReflectorCompositorAllocate(&PBRSharedContext->ReflectorCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumCompositorFree(PBRSharedContext->SpectrumCompositor);
        PhysxBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
        PhysxVisibilityTesterDestroy(&PBRSharedContext->VisibilityTester);
        return Status;
    }
    
    PBRSharedContext->TestGeometryRoutine = NULL;
    PBRSharedContext->TestGeometryRoutineContext = NULL;
    PBRSharedContext->LightList = NULL;
    PBRSharedContext->Rng = NULL;
    PBRSharedContext->Epsilon = (FLOAT) 0.0f;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRSharedContextSet(
    _Inout_ PPBR_SHARED_CONTEXT PBRSharedContext,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Epsilon
    )
{    
    ASSERT(PBRSharedContext != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));

    ReflectorCompositorClear(PBRSharedContext->ReflectorCompositor);
    SpectrumCompositorClear(PBRSharedContext->SpectrumCompositor);
    PhysxBrdfAllocatorFreeAll(&PBRSharedContext->BrdfAllocator);

    PhysxVisibilityTesterSetSceneAndEpsilon(&PBRSharedContext->VisibilityTester,
                                            TestGeometryRoutine,
                                            TestGeometryRoutineContext,
                                            Epsilon);

    PBRSharedContext->TestGeometryRoutine = TestGeometryRoutine;
    PBRSharedContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
    PBRSharedContext->LightList = LightList;
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
    PhysxBrdfAllocatorDestroy(&PBRSharedContext->BrdfAllocator);
    PhysxVisibilityTesterDestroy(&PBRSharedContext->VisibilityTester);
}

#endif // _PBR_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_
