/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the PHYSX_RAYTRACER type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_RAYTRACER_SHARED_CONTEXT {
    PPHYSX_VISIBILITY_TESTER VisibilityTester;
    PPHYSX_BRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    PCPHYSX_LIGHT_LIST LightList;
    PRANDOM_REFERENCE Rng;
    FLOAT Epsilon;
} PHYSX_RAYTRACER_SHARED_CONTEXT, *PPHYSX_RAYTRACER_SHARED_CONTEXT;

struct _PHYSX_RAYTRACER {
    PPHYSX_RAYTRACER NextRayTracer;
    PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext;
    PRAYTRACER RayTracer;
};

typedef struct _PHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT {
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
} PHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT, *PPHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT;

typedef CONST PHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT *PCPHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT;

typedef struct _PHYSX_RAYTRACER_PROCESS_HIT_CONTEXT {
    PPHYSX_RAYTRACER NextRayTracer;
    PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext;
    PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine;
    PVOID ProcessHitContext;
    RAY WorldRay;
    PCSPECTRUM *Output;
} PHYSX_RAYTRACER_PROCESS_HIT_CONTEXT, *PPHYSX_RAYTRACER_PROCESS_HIT_CONTEXT;

//
// Static Test Geometry Functions
//

SFORCEINLINE
VOID
PhysxRayTracerTestGeometryContextInitialize(
    _Out_ PPHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT Context,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext
    )
{
    ASSERT(Context != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    
    Context->TestGeometryRoutine = TestGeometryRoutine;
    Context->TestGeometryRoutineContext = TestGeometryRoutineContext;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxRayTracerTestGeometryCallback(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    )
{
    PCPHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT TestGeometryContext;
    PHYSX_HIT_TESTER PhysxHitTester;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(HitTester != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    PhysxHitTesterInitialize(&PhysxHitTester,
                             HitTester);
    
    TestGeometryContext = (PCPHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT) Context;
    
    Status = TestGeometryContext->TestGeometryRoutine(TestGeometryContext->TestGeometryRoutineContext,
                                                      &PhysxHitTester,
                                                      Ray);

    return Status;
}

//
// Static Process Hit Functions
//

SFORCEINLINE
VOID
PhysxRayTracerProcessHitContextInitialize(
    _Out_ PPHYSX_RAYTRACER_PROCESS_HIT_CONTEXT Context,
    _In_opt_ PPHYSX_RAYTRACER NextRayTracer,
    _In_ PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _In_opt_ PVOID ProcessHitContext,
    _In_ RAY WorldRay,
    _In_ PCSPECTRUM *Output
    )
{
    ASSERT(Context != NULL);
    ASSERT(SharedContext != NULL);
    ASSERT(ProcessHitRoutine != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(Output != NULL);
    
    Context->NextRayTracer = NextRayTracer;
    Context->SharedContext = SharedContext;
    Context->ProcessHitRoutine = ProcessHitRoutine;
    Context->ProcessHitContext = ProcessHitContext;
    Context->WorldRay = WorldRay;
    Context->Output = Output;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxRayTracerProcessHitCallback(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositorReference;
    PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositorReference;
    PPHYSX_RAYTRACER_PROCESS_HIT_CONTEXT ProcessHitContext;
    PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext;
    PCPHYSX_GEOMETRY Geometry;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);
    ASSERT(VectorValidate(ModelViewer) != FALSE);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(PointValidate(WorldHitPoint) != FALSE);
    
    ProcessHitContext = (PPHYSX_RAYTRACER_PROCESS_HIT_CONTEXT) Context;
    
    SharedContext = ProcessHitContext->SharedContext;

    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;
    SpectrumCompositorReference = SpectrumCompositorGetSpectrumCompositorReference(SharedContext->SpectrumCompositor);
    ReflectorCompositorReference = ReflectorCompositorGetReflectorCompositorReference(SharedContext->ReflectorCompositor);

    Status = ProcessHitContext->ProcessHitRoutine(ProcessHitContext->ProcessHitContext,
                                                  Geometry,
                                                  Hit->FrontFace,
                                                  ModelToWorld,
                                                  Hit->AdditionalData,
                                                  ModelViewer,
                                                  ModelHitPoint,
                                                  WorldHitPoint,
                                                  ProcessHitContext->WorldRay,
                                                  SharedContext->LightList,
                                                  ProcessHitContext->NextRayTracer,
                                                  SharedContext->VisibilityTester,
                                                  SharedContext->BrdfAllocator,
                                                  SpectrumCompositorReference,
                                                  ReflectorCompositorReference,
                                                  SharedContext->Rng,
                                                  ProcessHitContext->Output);
    
    return Status;
}

//
// Static Shared Context Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxRayTracerSharedContextAllocate(
    _Out_ PPHYSX_RAYTRACER_SHARED_CONTEXT *Context
    )
{   
    PPHYSX_RAYTRACER_SHARED_CONTEXT AllocatedContext;
    PVOID Allocation;
    ISTATUS Status;
     
    ASSERT(Context != NULL);
    
    Allocation = malloc(sizeof(PHYSX_RAYTRACER_SHARED_CONTEXT));

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedContext = (PPHYSX_RAYTRACER_SHARED_CONTEXT) Allocation;

    Status = PhysxVisibilityTesterAllocate(&AllocatedContext->VisibilityTester);
    
    if (Status != ISTATUS_SUCCESS)
    {
        free(AllocatedContext);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }
    
    Status = PhysxBrdfAllocatorCreate(&AllocatedContext->BrdfAllocator);
    
    if (Status != ISTATUS_SUCCESS)
    {
        PhysxVisibilityTesterFree(AllocatedContext->VisibilityTester);
        free(AllocatedContext);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }
    
    Status = SpectrumCompositorAllocate(&AllocatedContext->SpectrumCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        PhysxBrdfAllocatorFree(AllocatedContext->BrdfAllocator);
        PhysxVisibilityTesterFree(AllocatedContext->VisibilityTester);
        free(AllocatedContext);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }
    
    Status = ReflectorCompositorAllocate(&AllocatedContext->ReflectorCompositor);
    
    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumCompositorFree(AllocatedContext->SpectrumCompositor);
        PhysxBrdfAllocatorFree(AllocatedContext->BrdfAllocator);
        PhysxVisibilityTesterFree(AllocatedContext->VisibilityTester);
        free(AllocatedContext);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }
    
    AllocatedContext->TestGeometryRoutine = NULL;
    AllocatedContext->TestGeometryRoutineContext = NULL;
    AllocatedContext->LightList = NULL;
    AllocatedContext->Rng = NULL;
    AllocatedContext->Epsilon = (FLOAT) 0.0f;

    *Context = AllocatedContext;
    
    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PhysxRayTracerSharedContextSet(
    _Inout_ PPHYSX_RAYTRACER_SHARED_CONTEXT Context,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Epsilon
    )
{    
    ASSERT(Context != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));

    ReflectorCompositorClear(Context->ReflectorCompositor);
    SpectrumCompositorClear(Context->SpectrumCompositor);
    PhysxBrdfAllocatorFreeAll(Context->BrdfAllocator);

    PhysxVisibilityTesterSetSceneAndEpsilon(Context->VisibilityTester,
                                            TestGeometryRoutine,
                                            TestGeometryRoutineContext,
                                            Epsilon);

    Context->TestGeometryRoutine = TestGeometryRoutine;
    Context->TestGeometryRoutineContext = TestGeometryRoutineContext;
    Context->LightList = LightList;
    Context->Rng = Rng;
    Context->Epsilon = Epsilon;
}

SFORCEINLINE
VOID
PhysxRayTracerSharedContextFree(
    _In_opt_ _Post_invalid_ PPHYSX_RAYTRACER_SHARED_CONTEXT Context
    )
{
    if (Context == NULL)
    {
        return;
    }

    ReflectorCompositorFree(Context->ReflectorCompositor);
    SpectrumCompositorFree(Context->SpectrumCompositor);
    PhysxBrdfAllocatorFree(Context->BrdfAllocator);
    PhysxVisibilityTesterFree(Context->VisibilityTester);
    free(Context);
}

//
// Static Ray Tracer Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxRayTracerAllocateInternal(
    _In_ SIZE_T MaximumDepth,
    _Out_opt_ PPHYSX_RAYTRACER_SHARED_CONTEXT *SharedContext,
    _Out_ PPHYSX_RAYTRACER *RayTracer
    )
{
    PPHYSX_RAYTRACER AllocatedRayTracer;
    PVOID Allocation;
    ISTATUS Status;

    ASSERT(RayTracer != NULL);

    Allocation = malloc(sizeof(PHYSX_RAYTRACER));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedRayTracer = (PPHYSX_RAYTRACER) Allocation;

    Status = RayTracerAllocate(&AllocatedRayTracer->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        free(AllocatedRayTracer);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    if (MaximumDepth != 0)
    {
        Status = PhysxRayTracerAllocateInternal(MaximumDepth - 1,
                                                &AllocatedRayTracer->SharedContext,
                                                &AllocatedRayTracer->NextRayTracer);
    }
    else
    {
        AllocatedRayTracer->NextRayTracer = NULL;
        Status = PhysxRayTracerSharedContextAllocate(&AllocatedRayTracer->SharedContext);
    }

    if (Status != ISTATUS_SUCCESS)
    {
        RayTracerFree(AllocatedRayTracer->RayTracer);
        free(AllocatedRayTracer);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    if (SharedContext != NULL)
    {
        *SharedContext = AllocatedRayTracer->SharedContext;
    }
    
    *RayTracer = AllocatedRayTracer;

    return ISTATUS_SUCCESS;
}

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxRayTracerAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPHYSX_RAYTRACER *RayTracer
    )
{
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);
    
    Status = PhysxRayTracerAllocateInternal(MaximumDepth,
                                            NULL,
                                            RayTracer);

    return Status;
}

VOID
PhysxRayTracerConfigure(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(RayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon));

    PhysxRayTracerSharedContextSet(RayTracer->SharedContext,
                                   TestGeometryRoutine,
                                   TestGeometryRoutineContext,
                                   LightList,
                                   Rng,
                                   Epsilon);
}

VOID
PhysxRayTracerFree(
    _In_opt_ _Post_invalid_ PPHYSX_RAYTRACER RayTracer
    )
{
    if (RayTracer == NULL)
    {
        return;
    }
    
    if (RayTracer->NextRayTracer == NULL)
    {
        PhysxRayTracerSharedContextFree(RayTracer->SharedContext);
    }
    else
    {
        PhysxRayTracerFree(RayTracer->NextRayTracer);
    }
    
    RayTracerFree(RayTracer->RayTracer);
    free(RayTracer);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxRayTracerTraceSceneProcessClosestHit(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    )
{
    PHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT IntermediateTestGeometryContext;
    PHYSX_RAYTRACER_PROCESS_HIT_CONTEXT IntermediateProcessHitContext;
    PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext;
    PCSPECTRUM Result;
    ISTATUS Status;
    
    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    SharedContext = RayTracer->SharedContext;
    Result = NULL;

    PhysxRayTracerTestGeometryContextInitialize(&IntermediateTestGeometryContext,
                                                SharedContext->TestGeometryRoutine,
                                                SharedContext->TestGeometryRoutineContext);

    PhysxRayTracerProcessHitContextInitialize(&IntermediateProcessHitContext,
                                              RayTracer->NextRayTracer,
                                              SharedContext,
                                              ProcessHitRoutine,
                                              ProcessHitContext,
                                              Ray,
                                              &Result);

    Status = RayTracerTraceSceneProcessClosestHitWithCoordinates(RayTracer->RayTracer,
                                                                 Ray,
                                                                 SharedContext->Epsilon,
                                                                 PhysxRayTracerTestGeometryCallback,
                                                                 &IntermediateTestGeometryContext,
                                                                 PhysxRayTracerProcessHitCallback,
                                                                 &IntermediateProcessHitContext);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Spectrum = Result;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    )
{
    PHYSX_RAYTRACER_TEST_GEOMETRY_CONTEXT IntermediateTestGeometryContext;
    PHYSX_RAYTRACER_PROCESS_HIT_CONTEXT IntermediateProcessHitContext;
    PPHYSX_RAYTRACER_SHARED_CONTEXT SharedContext;
    PCSPECTRUM Result;
    ISTATUS Status;
    
    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    SharedContext = RayTracer->SharedContext;
    Result = NULL;

    PhysxRayTracerTestGeometryContextInitialize(&IntermediateTestGeometryContext,
                                                SharedContext->TestGeometryRoutine,
                                                SharedContext->TestGeometryRoutineContext);

    PhysxRayTracerProcessHitContextInitialize(&IntermediateProcessHitContext,
                                              RayTracer->NextRayTracer,
                                              SharedContext,
                                              ProcessHitRoutine,
                                              ProcessHitContext,
                                              Ray,
                                              &Result);

    Status = RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(RayTracer->RayTracer,
                                                                     Ray,
                                                                     PhysxRayTracerTestGeometryCallback,
                                                                     &IntermediateTestGeometryContext,
                                                                     PhysxRayTracerProcessHitCallback,
                                                                     &IntermediateProcessHitContext);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Spectrum = Result;

    return ISTATUS_SUCCESS;
}
