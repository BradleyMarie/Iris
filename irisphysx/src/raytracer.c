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
    PPHYSX_RAYTRACER NextRayTracer;
    PPBR_SHARED_CONTEXT SharedContext;
    PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine;
    PVOID ProcessHitContext;
    RAY WorldRay;
    PCSPECTRUM *Output;
} RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT, *PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT;

//
// Static Functions
//

SFORCEINLINE
VOID
PhysxRayTracerProcessHitAdapterContextInitialize(
    _Out_ PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext,
    _In_ PPHYSX_RAYTRACER NextRayTracer,
    _In_ PPBR_SHARED_CONTEXT SharedContext,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
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
    
    AdapterContext->NextRayTracer = NextRayTracer;
    AdapterContext->SharedContext = SharedContext;
    AdapterContext->ProcessHitRoutine = ProcessHitRoutine;
    AdapterContext->ProcessHitContext = ProcessHitContext;
    AdapterContext->WorldRay = WorldRay;
    AdapterContext->Output = Output;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxRayTracerProcessHitAdapter(
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
    PCPHYSX_GEOMETRY Geometry;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);
    
    AdapterContext = (PRAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT) Context;
    
    SharedContext = AdapterContext->SharedContext;

    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;
    SpectrumCompositorReference = SpectrumCompositorGetSpectrumCompositorReference(SharedContext->SpectrumCompositor);
    ReflectorCompositorReference = ReflectorCompositorGetReflectorCompositorReference(SharedContext->ReflectorCompositor);

    Status = AdapterContext->ProcessHitRoutine(AdapterContext->ProcessHitContext,
                                               Geometry,
                                               Hit->FrontFace,
                                               ModelToWorld,
                                               Hit->AdditionalData,
                                               ModelViewer,
                                               ModelHitPoint,
                                               WorldHitPoint,
                                               AdapterContext->WorldRay,
                                               SharedContext->LightList,
                                               AdapterContext->NextRayTracer,
                                               &SharedContext->VisibilityTester,
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
PhysxRayTracerTraceSceneProcessClosestHit(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    )
{
    RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext;
    PPBR_SHARED_CONTEXT SharedContext;
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

    PhysxRayTracerProcessHitAdapterContextInitialize(&AdapterContext,
                                                     RayTracer->NextRayTracer,
                                                     SharedContext,
                                                     ProcessHitRoutine,
                                                     ProcessHitContext,
                                                     Ray,
                                                     &Result);

    Status = RayTracerAdapterTraceSceneProcessClosestHitWithCoordinates(RayTracer->RayTracer,
                                                                        Ray,
                                                                        SharedContext->Epsilon,
                                                                        SharedContext->TestGeometryRoutine,
                                                                        SharedContext->TestGeometryRoutineContext,
                                                                        PhysxRayTracerProcessHitAdapter,
                                                                        &AdapterContext);

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
    RAYTRACER_PROCESS_HIT_ADAPTER_CONTEXT AdapterContext;
    PPBR_SHARED_CONTEXT SharedContext;
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
    
    PhysxRayTracerProcessHitAdapterContextInitialize(&AdapterContext,
                                                     RayTracer->NextRayTracer,
                                                     SharedContext,
                                                     ProcessHitRoutine,
                                                     ProcessHitContext,
                                                     Ray,
                                                     &Result);

    Status = RayTracerAdapterTraceSceneProcessAllHitsInOrderWithCoordinates(RayTracer->RayTracer,
                                                                            Ray,
                                                                            SharedContext->TestGeometryRoutine,
                                                                            SharedContext->TestGeometryRoutineContext,
                                                                            PhysxRayTracerProcessHitAdapter,
                                                                            &AdapterContext);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Spectrum = Result;

    return ISTATUS_SUCCESS;
}
