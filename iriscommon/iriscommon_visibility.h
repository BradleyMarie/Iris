/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_visibility.h

Abstract:

    This file contains the function definitions for the visibility routines.

--*/

#ifndef _IRIS_COMMON_VISIBILITY_
#define _IRIS_COMMON_VISIBILITY_

#include <iris.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerOwnerTestVisibility(
    _In_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ RAY WorldRay,
    _In_ FLOAT Epsilon,
    _In_ PCSCENE Scene,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    ASSERT(RayTracerOwner != NULL);
    ASSERT(IsNormalFloat(Epsilon) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(Scene != NULL);
    ASSERT(IsNormalFloat(DistanceToObject) != FALSE);
    ASSERT(IsFiniteFloat(DistanceToObject) != FALSE);
    ASSERT(DistanceToObject >= (FLOAT) 0.0f);
    ASSERT(Visible != NULL);

    Status = RayTracerOwnerGetRayTracer(RayTracerOwner,
                                        WorldRay,
                                        TRUE,
                                        &RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Scene, RayTracer);

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

    if (Epsilon < DistanceToObject &&
        IsFiniteFloat(DistanceToObject) != FALSE)
    {
        DistanceToObject -= Epsilon;
    }

    do
    {
        if (Epsilon < ShapeHit->Distance &&
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
SFORCEINLINE
ISTATUS
RayTracerOwnerTestVisibilityAnyDistance(
    _In_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ RAY WorldRay,
    _In_ FLOAT Epsilon,
    _In_ PCSCENE Scene,
    _Out_ PBOOL Visible
    )
{
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    ASSERT(RayTracerOwner != NULL);
    ASSERT(IsNormalFloat(Epsilon) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(Scene != NULL);
    ASSERT(Visible != NULL);

    Status = RayTracerOwnerGetRayTracer(RayTracerOwner,
                                        WorldRay,
                                        TRUE,
                                        &RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Scene, RayTracer);

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
        if (Epsilon < ShapeHit->Distance)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
}

#endif // _IRIS_COMMON_VISIBILITY_