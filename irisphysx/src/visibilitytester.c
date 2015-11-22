/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the 
    SPECTRUM_VISIBILITY_TESTER type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumVisibilityTesterTestVisibility(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerTestOwnerVisibility
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

    Status = RayTracerOwnerTestVisibility(Tester->RayTracerOwner,
                                          WorldRay,
                                          Tester->Epsilon,
                                          Tester->Scene,
                                          DistanceToObject,
                                          Visible);

    return ISTATUS_SUCCESS;
}

//
// This is needed since not all C environments support the INFINITY macro
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumVisibilityTesterTestVisibilityAnyDistance(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerTestOwnerVisibilityAnyDistance
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = RayTracerOwnerTestVisibilityAnyDistance(Tester->RayTracerOwner,
                                                     WorldRay,
                                                     Tester->Epsilon,
                                                     Tester->Scene,
                                                     Visible);

    return ISTATUS_SUCCESS;
}