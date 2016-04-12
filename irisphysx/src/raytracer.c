/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the PBR_RAYTRACER type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef struct _RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT {
    PPBR_RAYTRACER NextPRBRayTracer;
    PPBR_SHARED_CONTEXT SharedContext;
    PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine;
    PVOID ProcessHitContext;
    RAY WorldRay;
    PCSPECTRUM *Output;
} RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT, *PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT;

//
// Static Functions
//

SFORCEINLINE
VOID
PBRRayTracerProcessHitAdapterContextInitialize(
    _Out_ PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext,
    _In_ PPBR_RAYTRACER NextPRBRayTracer,
    _In_ PPBR_SHARED_CONTEXT SharedContext,
    _In_ PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _In_opt_ PVOID ProcessHitContext,
    _In_ RAY WorldRay,
    _In_ PCSPECTRUM *Output
    )
{
    ASSERT(AdapterContext != NULL);
    ASSERT(SharedContext != NULL);
    ASSERT(ProcessHitRoutine != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(Output != NULL);
    
    AdapterContext->NextPRBRayTracer = NextPRBRayTracer;
    AdapterContext->SharedContext = SharedContext;
    AdapterContext->ProcessHitRoutine = ProcessHitRoutine;
    AdapterContext->ProcessHitContext = ProcessHitContext;
    AdapterContext->Output = Output;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PBRRayTracerProcessHitAdapter(
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
    PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext;
    PPBR_SHARED_CONTEXT SharedContext;
    PCPBR_GEOMETRY PBRGeometry;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);
    
    AdapterContext = (PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT) Context;
    
    SharedContext = AdapterContext->SharedContext;

    PBRGeometry = (PCPBR_GEOMETRY) Hit->Data;
    SpectrumCompositorReference = SpectrumCompositorGetSpectrumCompositorReference(SharedContext->SpectrumCompositor);
    ReflectorCompositorReference = ReflectorCompositorGetReflectorCompositorReference(SharedContext->ReflectorCompositor);

    Status = AdapterContext->ProcessHitRoutine(AdapterContext->ProcessHitContext,
                                               PBRGeometry,
                                               ModelToWorld,
                                               ModelViewer,
                                               ModelHitPoint,
                                               WorldHitPoint,
                                               AdapterContext->WorldRay,
                                               SharedContext->Lights,
                                               SharedContext->NumberOfLights,
                                               AdapterContext->NextPRBRayTracer,
                                               &SharedContext->PBRVisibilityTester,
                                               &SharedContext->BrdfAllocator,
                                               SpectrumCompositorReference,
                                               ReflectorCompositorReference,
                                               SharedContext->Rng,
                                               AdapterContext->Output);
    
    return Status;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRRayTracerTraceSceneProcessClosestHit(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray,
    _In_ PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    )
{
    RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext;
    PPBR_SHARED_CONTEXT SharedContext;
    ISTATUS Status;
    
    if (PBRRayTracer == NULL)
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
    
    SharedContext = PBRRayTracer->SharedContext;

    PBRRayTracerProcessHitAdapterContextInitialize(&AdapterContext,
                                                   PBRRayTracer->NextPRBRayTracer,
                                                   SharedContext,
                                                   ProcessHitRoutine,
                                                   ProcessHitContext,
                                                   Ray,
                                                   Spectrum);

    Status = RayTracerAdapterTraceSceneProcessClosestHitWithCoordinates(PBRRayTracer->RayTracer,
                                                                        Ray,
                                                                        SharedContext->Epsilon,
                                                                        SharedContext->TestGeometryRoutine,
                                                                        SharedContext->TestGeometryRoutineContext,
                                                                        PBRRayTracerProcessHitAdapter,
                                                                        &AdapterContext);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray,
    _In_ PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    )
{
    RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext;
    PPBR_SHARED_CONTEXT SharedContext;
    ISTATUS Status;
    
    if (PBRRayTracer == NULL)
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

    SharedContext = PBRRayTracer->SharedContext;
    
    PBRRayTracerProcessHitAdapterContextInitialize(&AdapterContext,
                                                   PBRRayTracer->NextPRBRayTracer,
                                                   SharedContext,
                                                   ProcessHitRoutine,
                                                   ProcessHitContext,
                                                   Ray,
                                                   Spectrum);

    Status = RayTracerAdapterTraceSceneProcessAllHitsInOrderWithCoordinates(PBRRayTracer->RayTracer,
                                                                            Ray,
                                                                            SharedContext->TestGeometryRoutine,
                                                                            SharedContext->TestGeometryRoutineContext,
                                                                            PBRRayTracerProcessHitAdapter,
                                                                            &AdapterContext);

    return Status;
}