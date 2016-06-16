/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This module implements the raytracer functions.

--*/

#include <irisp.h>

//
// Types
//

struct _RAYTRACER {
    HIT_TESTER HitTester;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerAllocate(
    _Out_ PRAYTRACER *Result
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    RayTracer = (PRAYTRACER) malloc(sizeof(RAYTRACER));

    if (RayTracer == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = HitTesterInitialize(&RayTracer->HitTester);

    if (Status != ISTATUS_SUCCESS)
    {
        free(RayTracer);
        return Status;
    }

    *Result = RayTracer;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT CurrentHit;
    PCINTERNAL_HIT ClosestHit;
    FLOAT ClosestDistance;
    FLOAT CurrentDistance;
    PHIT_TESTER HitTester;
    ISTATUS Status;
    BOOL FoundHit;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (IsLessThanZeroFloat(MinimumDistance) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }
    
    HitTester = &RayTracer->HitTester;

    HitTesterSetRay(HitTester, Ray);

    Status = TestShapesRoutine(TestShapesContext, 
                               HitTester,
                               Ray);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    FoundHit = FALSE;

    Status = HitTesterGetNextHit(HitTester, &ClosestHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        if (ClosestHit->Hit.Distance > MinimumDistance)
        {
            FoundHit = TRUE;
            break;
        }

        Status = HitTesterGetNextHit(HitTester, &ClosestHit);
    }
    
    if (FoundHit == FALSE)
    {
        return ISTATUS_SUCCESS;
    }

    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = HitTesterGetNextHit(HitTester,
                                     &CurrentHit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        CurrentDistance = ClosestHit->Hit.Distance;
        
        if (CurrentDistance > MinimumDistance &&
            CurrentDistance < ClosestDistance)
        {
            ClosestDistance = CurrentDistance;
            ClosestHit = CurrentHit;
        }
    }
    
    Status = ProcessHitRoutine(ProcessHitContext,
                               &ClosestHit->Hit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT ClosestHit;
    PCINTERNAL_HIT CurrentHit;
    PCMATRIX ModelToWorld;
    FLOAT ClosestDistance;
    FLOAT CurrentDistance;
    PHIT_TESTER HitTester;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;
    BOOL FoundHit;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (IsLessThanZeroFloat(MinimumDistance) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    HitTester = &RayTracer->HitTester;
    
    HitTesterSetRay(HitTester, Ray);

    Status = TestShapesRoutine(TestShapesContext, 
                               HitTester,
                               Ray);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    FoundHit = FALSE;

    Status = HitTesterGetNextHit(HitTester, &ClosestHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        if (ClosestHit->Hit.Distance > MinimumDistance)
        {
            FoundHit = TRUE;
            break;
        }

        Status = HitTesterGetNextHit(HitTester, &ClosestHit);
    }
    
    if (FoundHit == FALSE)
    {
        return ISTATUS_SUCCESS;
    }

    ClosestDistance = ClosestHit->Hit.Distance;
    
    while (TRUE)
    {
        Status = HitTesterGetNextHit(HitTester,
                                     &CurrentHit);
                                          
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        CurrentDistance = CurrentHit->Hit.Distance;
        
        if (CurrentDistance > MinimumDistance &&
            CurrentDistance < ClosestDistance)
        {
            ClosestDistance = CurrentDistance;
            ClosestHit = CurrentHit;
        }
    }
    
    HitTesterComputeHitData(HitTester,
                            ClosestHit,
                            &ModelToWorld,
                            &ModelViewer,
                            &ModelHit,
                            &WorldHit);
    
    Status = ProcessHitRoutine(ProcessHitContext,
                               &ClosestHit->Hit,
                               ModelToWorld,
                               ModelViewer,
                               ModelHit,
                               WorldHit);
    
    if (Status == ISTATUS_NO_MORE_DATA)
    {
        return ISTATUS_SUCCESS;
    }
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCINTERNAL_HIT CurrentHit;
    PHIT_TESTER HitTester;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    HitTester = &RayTracer->HitTester;
    
    HitTesterSetRay(HitTester, Ray);

    Status = TestShapesRoutine(TestShapesContext, 
                               HitTester,
                               Ray);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    Status = HitTesterGetNextHit(HitTester,
                                 &CurrentHit);
    
    while (Status != ISTATUS_NO_MORE_DATA)
    {                            
        Status = ProcessHitRoutine(ProcessHitContext,
                                   &CurrentHit->Hit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Status = HitTesterGetNextHit(HitTester,
                                     &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PCMATRIX ModelToWorld;
    PCINTERNAL_HIT CurrentHit;
    PHIT_TESTER HitTester;
    VECTOR3 ModelViewer;
    POINT3 ModelHit;
    POINT3 WorldHit;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(Ray) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    HitTester = &RayTracer->HitTester;
    
    HitTesterSetRay(HitTester, Ray);

    Status = TestShapesRoutine(TestShapesContext, 
                               HitTester,
                               Ray);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    HitTesterSort(HitTester);
    
    Status = HitTesterGetNextHit(HitTester,
                                 &CurrentHit);

    while (Status != ISTATUS_NO_MORE_DATA)
    {
        HitTesterComputeHitData(HitTester,
                                CurrentHit,
                                &ModelToWorld,
                                &ModelViewer,
                                &ModelHit,
                                &WorldHit);

        Status = ProcessHitRoutine(ProcessHitContext,
                                   &CurrentHit->Hit,
                                   ModelToWorld,
                                   ModelViewer,
                                   ModelHit,
                                   WorldHit);
        
        if (Status == ISTATUS_NO_MORE_DATA)
        {
            break;
        }
        
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Status = HitTesterGetNextHit(HitTester,
                                     &CurrentHit);
    }
    
    return ISTATUS_SUCCESS;
}

IRISAPI
VOID
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER RayTracer
    )
{
    if (RayTracer == NULL)
    {
        return;
    }

    HitTesterDestroy(&RayTracer->HitTester);
    free(RayTracer);
}