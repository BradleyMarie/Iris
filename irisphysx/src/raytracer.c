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
SpectrumRayTracerTraceSceneFindClosestHit(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE SpectrumProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE ProcessHitRoutine;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer->Scene != NULL);

    if (SpectrumRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    ProcessHitRoutine = (PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE) SpectrumProcessHitRoutine;
    
    Status = RayTracerOwnerTraceSceneFindClosestHitWithData(SpectrumRayTracer->RayTracerOwner,
                                                            SpectrumRayTracer->Scene->Scene,
                                                            Ray,
                                                            MinimumDistance,
                                                            ProcessHitRoutine,
                                                            ProcessHitRoutineContext);
                                                            
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumRayTracerTraceSceneFindAllHits(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE SpectrumProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitRoutineContext
    )
{
    PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE ProcessHitRoutine;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer->Scene != NULL);

    if (SpectrumRayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    ProcessHitRoutine = (PRAYTRACER_PROCESS_HIT_WITH_DATA_ROUTINE) SpectrumProcessHitRoutine;
    
    Status = RayTracerOwnerTraceSceneFindAllHits(SpectrumRayTracer->RayTracerOwner,
                                                 SpectrumRayTracer->Scene->Scene,
                                                 Ray,
                                                 ProcessHitRoutine,
                                                 ProcessHitRoutineContext);
                                                            
    return Status;
}