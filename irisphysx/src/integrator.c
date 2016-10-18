/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    integrator.c

Abstract:

    This file contains the definitions for the PHYSX_INTEGRATOR type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_INTEGRATOR {
    PHYSX_SHARED_CONTEXT SharedContext;
    PPHYSX_RAYTRACER NextRayTracer;
};

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxIntegratorAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPHYSX_INTEGRATOR *Result
    )
{
    PPHYSX_INTEGRATOR Integrator;
    PPHYSX_RAYTRACER RayTracer;
    PVOID Allocation;
    SIZE_T Index;
    ISTATUS Status;
    
    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Allocation = malloc(sizeof(PHYSX_INTEGRATOR));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    Integrator = (PPHYSX_INTEGRATOR) Allocation;
    
    Status = PhysxSharedContextInitialize(&Integrator->SharedContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        free(Allocation);
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    RayTracer = NULL;
    
    for (Index = 0; Index <= MaximumDepth; Index++)
    {
        Status = PhysxRayTracerAllocate(RayTracer,
                                        &Integrator->SharedContext,
                                        &RayTracer);
    
        if (Status != ISTATUS_SUCCESS)
        {
            PhysxRayTracerFree(RayTracer);
            PhysxSharedContextDestroy(&Integrator->SharedContext);
            free(Allocation);
            return ISTATUS_ALLOCATION_FAILED;
        }
    }
    
    Integrator->NextRayTracer = RayTracer;

    *Result = Integrator;
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxIntegratorIntegrate(
    _In_ PPHYSX_INTEGRATOR Integrator,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_ PPHYSX_INTEGRATOR_INTEGRATE_ROUTINE IntegrateRoutine,
    _Inout_opt_ PVOID IntegrateRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ FLOAT Epsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM_REFERENCE Rng
    )
{
    ISTATUS Status;
    
    if (Integrator == NULL)
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
    
    PhysxSharedContextSet(&Integrator->SharedContext,
                          TestGeometryRoutine,
                          TestGeometryRoutineContext,
                          LightList,
                          Rng,
                          Epsilon);

    Status = IntegrateRoutine(IntegrateRoutineContext,
                              Integrator->NextRayTracer,
                              WorldRay);

    return Status;
}

VOID
PhysxIntegratorFree(
    _In_opt_ _Post_invalid_ PPHYSX_INTEGRATOR Integrator
    )
{
    if (Integrator == NULL)
    {
        return;
    }
    
    PhysxRayTracerFree(Integrator->NextRayTracer);
    PhysxSharedContextDestroy(&Integrator->SharedContext);
    free(Integrator);
}
