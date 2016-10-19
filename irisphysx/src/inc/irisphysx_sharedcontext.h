/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_sharedcontext.h

Abstract:

    This file contains the internal definitions for the 
    PHYSX_SHARED_CONTEXT type.

--*/

#ifndef _PHYSX_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_
#define _PHYSX_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_SHARED_CONTEXT {
    PPHYSX_VISIBILITY_TESTER VisibilityTester;
    PPHYSX_BRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    PCPHYSX_LIGHT_LIST LightList;
    PRANDOM_REFERENCE Rng;
    FLOAT Epsilon;
} PHYSX_SHARED_CONTEXT, *PPHYSX_SHARED_CONTEXT;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxSharedContextInitialize(
    _Out_ PPHYSX_SHARED_CONTEXT SharedContext
    )
{   
    ISTATUS Status;
     
    ASSERT(SharedContext != NULL);
    
    Status = PhysxVisibilityTesterAllocate(&SharedContext->VisibilityTester);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = PhysxBrdfAllocatorCreate(&SharedContext->BrdfAllocator);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    Status = SpectrumCompositorAllocate(&SharedContext->SpectrumCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        PhysxBrdfAllocatorFree(SharedContext->BrdfAllocator);
        PhysxVisibilityTesterFree(SharedContext->VisibilityTester);
        return Status;
    }
    
    Status = ReflectorCompositorAllocate(&SharedContext->ReflectorCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumCompositorFree(SharedContext->SpectrumCompositor);
        PhysxBrdfAllocatorFree(SharedContext->BrdfAllocator);
        PhysxVisibilityTesterFree(SharedContext->VisibilityTester);
        return Status;
    }
    
    SharedContext->TestGeometryRoutine = NULL;
    SharedContext->TestGeometryRoutineContext = NULL;
    SharedContext->LightList = NULL;
    SharedContext->Rng = NULL;
    SharedContext->Epsilon = (FLOAT) 0.0f;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PhysxSharedContextSet(
    _Inout_ PPHYSX_SHARED_CONTEXT SharedContext,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Epsilon
    )
{    
    ASSERT(SharedContext != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));

    ReflectorCompositorClear(SharedContext->ReflectorCompositor);
    SpectrumCompositorClear(SharedContext->SpectrumCompositor);
    PhysxBrdfAllocatorFreeAll(SharedContext->BrdfAllocator);

    PhysxVisibilityTesterSetSceneAndEpsilon(SharedContext->VisibilityTester,
                                            TestGeometryRoutine,
                                            TestGeometryRoutineContext,
                                            Epsilon);

    SharedContext->TestGeometryRoutine = TestGeometryRoutine;
    SharedContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
    SharedContext->LightList = LightList;
    SharedContext->Rng = Rng;
    SharedContext->Epsilon = Epsilon;
}

SFORCEINLINE
VOID
PhysxSharedContextDestroy(
    _In_ _Post_invalid_ PPHYSX_SHARED_CONTEXT SharedContext
    )
{
    ASSERT(SharedContext != NULL);
    
    ReflectorCompositorFree(SharedContext->ReflectorCompositor);
    SpectrumCompositorFree(SharedContext->SpectrumCompositor);
    PhysxBrdfAllocatorFree(SharedContext->BrdfAllocator);
    PhysxVisibilityTesterFree(SharedContext->VisibilityTester);
}

#endif // _PHYSX_SHARED_CONTEXT_IRIS_PHYSX_INTERNAL_
