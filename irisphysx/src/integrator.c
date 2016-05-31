/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    integrator.c

Abstract:

    This file contains the definitions for the PBR_INTEGRATOR type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PBR_INTEGRATOR {
    PBR_SHARED_CONTEXT PBRSharedContext;
    PPBR_RAYTRACER NextPBRRayTracer;
};

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRIntegratorAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPBR_INTEGRATOR *Result
    )
{
    PPBR_INTEGRATOR PBRIntegrator;
    PPBR_RAYTRACER PBRRayTracer;
    PVOID Allocation;
    SIZE_T Index;
    ISTATUS Status;
    
    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Allocation = malloc(sizeof(PBR_INTEGRATOR));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    PBRIntegrator = (PPBR_INTEGRATOR) Allocation;
    
    Status = PBRSharedContextInitialize(&PBRIntegrator->PBRSharedContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        free(Allocation);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    PBRRayTracer = NULL;
    
    for (Index = 0; Index <= MaximumDepth; Index++)
    {
        Status = PBRRayTracerAllocate(PBRRayTracer,
                                      &PBRIntegrator->PBRSharedContext,
                                      &PBRRayTracer);
    
        if (Status != ISTATUS_SUCCESS)
        {
            PBRRayTracerFree(PBRRayTracer);
            PBRSharedContextDestroy(&PBRIntegrator->PBRSharedContext);
            free(Allocation);
            return ISTATUS_ALLOCATION_FAILED;
        }
    }
    
    PBRIntegrator->NextPBRRayTracer = PBRRayTracer;

    *Result = PBRIntegrator;
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRIntegratorIntegrate(
    _In_ PPBR_INTEGRATOR PBRIntegrator,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_ PPBR_INTEGRATOR_INTEGRATE_ROUTINE IntegrateRoutine,
    _Inout_opt_ PVOID IntegrateRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ FLOAT Epsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM_REFERENCE Rng
    )
{
    ISTATUS Status;
    
    if (PBRIntegrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (IntegrateRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
 
    if (IsFiniteFloat(Epsilon) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Epsilon) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }
    
    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }
    
    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }
    
    PBRSharedContextSet(&PBRIntegrator->PBRSharedContext,
                        TestGeometryRoutine,
                        TestGeometryRoutineContext,
                        LightList,
                        Rng,
                        Epsilon);

    Status = IntegrateRoutine(IntegrateRoutineContext,
                              PBRIntegrator->NextPBRRayTracer,
                              WorldRay);

    return Status;
}

VOID
PBRIntegratorFree(
    _In_opt_ _Post_invalid_ PPBR_INTEGRATOR PBRIntegrator
    )
{
    if (PBRIntegrator == NULL)
    {
        return;
    }
    
    PBRRayTracerFree(PBRIntegrator->NextPBRRayTracer);
    PBRSharedContextDestroy(&PBRIntegrator->PBRSharedContext);
    free(PBRIntegrator);
}