/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumRayTracerTraceSceneProcessClosestHit(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE IrisProcessHitRoutine;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer->Scene != NULL);

    if (SpectrumRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    IrisProcessHitRoutine = (PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE) ProcessHitRoutine;
    
    Status = RayTracerOwnerTraceSceneProcessClosestHitWithCoordinates(SpectrumRayTracer->RayTracerOwner,
                                                                      SpectrumRayTracer->Scene->Scene,
                                                                      Ray,
                                                                      MinimumDistance,
                                                                      IrisProcessHitRoutine,
                                                                      ProcessHitContext);
                                                            
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE IrisProcessHitRoutine;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer->Scene != NULL);

    if (SpectrumRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    IrisProcessHitRoutine = (PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE) ProcessHitRoutine;
    
    Status = RayTracerOwnerTraceSceneProcessAllHitsInOrderWithCoordinates(SpectrumRayTracer->RayTracerOwner,
                                                                          SpectrumRayTracer->Scene->Scene,
                                                                          Ray,
                                                                          IrisProcessHitRoutine,
                                                                          ProcessHitContext);
                                                            
    return Status;
}