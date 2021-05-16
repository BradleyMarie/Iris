/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    visibility_tester.c

Abstract:

    Definitions for the visibility_tester interface.

--*/

#include "iris_physx/visibility_tester.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Static Functions
//

ISTATUS
VisibilityTesterProcessHit(
    _Inout_opt_ void *context,
    _In_ PCHIT_CONTEXT hit_context
    )
{
    bool *visible = (bool*)context;

    *visible = false;
    
    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
VisibilityTesterTest(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ float_t distance_to_object,
    _Out_ bool *visible
    )
{
    if (visibility_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (isless(distance_to_object, (float_t)0.0))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ISTATUS status = VisibilityTesterTestInline(visibility_tester,
                                                ray,
                                                distance_to_object,
                                                visible);

    return status;
}

ISTATUS
VisibilityTesterTestAnyDistance(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _Out_ bool *visible
    )
{
    if (visibility_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                           ray,
                                                           visible);

    return status;
}