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
    PPHYSX_RAYTRACER RayTracer;
};

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxIntegratorAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPHYSX_INTEGRATOR *Integrator
    )
{
    PPHYSX_INTEGRATOR AllocatedIntegrator;
    PVOID Allocation;
    ISTATUS Status;
    
    if (Integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Allocation = malloc(sizeof(PHYSX_INTEGRATOR));
    
    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedIntegrator = (PPHYSX_INTEGRATOR) Allocation;
    
    Status = PhysxRayTracerAllocate(MaximumDepth,
                                    &AllocatedIntegrator->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(AllocatedIntegrator);
        return Status;
    }

    *Integrator = AllocatedIntegrator;
    
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
    _Inout_ PRANDOM Rng
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
    
    PhysxRayTracerConfigure(Integrator->RayTracer,
                            TestGeometryRoutine,
                            TestGeometryRoutineContext,
                            LightList,
                            Rng,
                            Epsilon);

    Status = IntegrateRoutine(IntegrateRoutineContext,
                              Integrator->RayTracer,
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
    
    PhysxRayTracerFree(Integrator->RayTracer);
    free(Integrator);
}
