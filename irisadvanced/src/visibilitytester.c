/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the VISIBILITY_TESTER type.

--*/

#include <irisadvancedp.h>

//
// Types
//

struct _VISIBILITY_TESTER {
    PRAYTRACER_OWNER RayTracerOwner;
    PRAYTRACER RayTracer;
    FLOAT Epsilon;
    PCSCENE Scene;
};

struct _VISIBILITY_TESTER_OWNER {
    VISIBILITY_TESTER VisibilityTester;  
};

//
// VisibilityTester Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
VisibilityTesterInitialize(
    _Out_ PVISIBILITY_TESTER Tester
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    VECTOR3 TemporaryDirection;
    POINT3 TemporaryOrigin;
    RAY TemporaryRay;
    ISTATUS Status;

    ASSERT(Tester != NULL);

    TemporaryDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    TemporaryOrigin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TemporaryRay = RayCreate(TemporaryOrigin, TemporaryDirection);

    Status = RayTracerOwnerAllocate(TemporaryRay, &RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Tester->Scene = NULL;
    Tester->RayTracerOwner = RayTracerOwner;
    Tester->RayTracer = RayTracerOwnerGetRayTracer(RayTracerOwner);
    Tester->Epsilon = (FLOAT) 0.0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
VisibilityTesterSetSceneAndEpsilon(
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(VisibilityTester != NULL);
    ASSERT(Scene != NULL);
    ASSERT(IsNormalFloat(Epsilon) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(Epsilon < (FLOAT) 0.0);
    
    VisibilityTester->Scene = Scene;
    VisibilityTester->Epsilon = Epsilon;
}

SFORCEINLINE
VOID
VisibilityTesterDestroy(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER Tester
    )
{
    ASSERT(Tester != NULL);

    RayTracerOwnerFree(Tester->RayTracerOwner);
}

//
// VisibilityTester Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VisibilityTesterTestVisibility(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    //
    // Ray is validated by RayTracerSetRay
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(DistanceToObject) == FALSE ||
        IsFiniteFloat(DistanceToObject) == FALSE ||
        DistanceToObject < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    RayTracerOwner = Tester->RayTracerOwner;

    Status = RayTracerOwnerSetRay(Tester->RayTracerOwner, 
                                  WorldRay, 
                                  TRUE);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
                        Tester->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        *Visible = TRUE;
        return ISTATUS_SUCCESS;
    }

    if (Tester->Epsilon < DistanceToObject &&
        IsFiniteFloat(DistanceToObject) != FALSE)
    {
        DistanceToObject -= Tester->Epsilon;
    }

    do
    {
        if (Tester->Epsilon < ShapeHit->Distance &&
            ShapeHit->Distance < DistanceToObject)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
}

//
// This is needed since not all C environments support the INFINITY macro
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VisibilityTesterTestVisibilityAnyDistance(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    //
    // Ray is validated by RayTracerSetRay
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    RayTracerOwner = Tester->RayTracerOwner;

    Status = RayTracerOwnerSetRay(RayTracerOwner, WorldRay, TRUE);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
                        Tester->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        *Visible = TRUE;
        return ISTATUS_SUCCESS;
    }

    do
    {
        if (Tester->Epsilon < ShapeHit->Distance)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
}

//
// VisibilityTesterOwner Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterOwnerAllocate(
    _Out_ PVISIBILITY_TESTER_OWNER *Result
    )
{
    PVISIBILITY_TESTER_OWNER VisibilityTesterOwner;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    VisibilityTesterOwner = (PVISIBILITY_TESTER_OWNER) malloc(sizeof(VISIBILITY_TESTER_OWNER));
    
    if (VisibilityTesterOwner == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    Status = VisibilityTesterInitialize(&VisibilityTesterOwner->VisibilityTester);
                                        
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterOwnerGetVisibilityTester(
    _In_ PVISIBILITY_TESTER_OWNER VisibilityTesterOwner,
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon,
    _Out_ PVISIBILITY_TESTER *VisibilityTester
    )
{
    if (VisibilityTesterOwner == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsNormalFloat(Epsilon) == FALSE ||
        IsFiniteFloat(Epsilon) == FALSE ||
        Epsilon < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (VisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    VisibilityTesterSetSceneAndEpsilon(&VisibilityTesterOwner->VisibilityTester,
                                       Scene,
                                       Epsilon);

    *VisibilityTester = &VisibilityTesterOwner->VisibilityTester;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterOwnerSetSceneAndEpsilon(
    _Inout_ PVISIBILITY_TESTER_OWNER VisibilityTesterOwner,
    _In_ PSCENE Scene,
    _In_ FLOAT Epsilon
    )
{
    if (VisibilityTesterOwner == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsNormalFloat(Epsilon) == FALSE ||
        IsFiniteFloat(Epsilon) == FALSE ||
        Epsilon < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    VisibilityTesterSetSceneAndEpsilon(&VisibilityTesterOwner->VisibilityTester,
                                       Scene,
                                       Epsilon);
                                       
    return ISTATUS_SUCCESS;
}

IRISADVANCEDAPI
VOID
VisibilityTesterOwnerFree(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER_OWNER Tester
    )
{
    if (Tester == NULL)
    {
        return;
    }
    
    VisibilityTesterDestroy(&Tester->VisibilityTester);
    free(Tester);
}