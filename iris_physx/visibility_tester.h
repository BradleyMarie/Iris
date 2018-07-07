/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    visibility_tester.h

Abstract:

    Allows testing the visibility of one point from another. A point is 
    considered not visible if there is a successful hit test anywhere between
    the two points.

--*/

#ifndef _IRIS_PHYSX_VISIBILITY_TESTER_
#define _IRIS_PHYSX_VISIBILITY_TESTER_

#include "iris/iris.h"

//
// Types
//

typedef struct _VISIBILITY_TESTER VISIBILITY_TESTER, *PVISIBILITY_TESTER;
typedef const VISIBILITY_TESTER *PCVISIBILITY_TESTER;

//
// Functions
//

ISTATUS
VisibilityTesterTest(
    _In_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ float_t distance_to_object,
    _Out_ bool *visible
    );

ISTATUS
VisibilityTesterTestAnyDistance(
    _In_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _Out_ bool *visible
    );

#endif // _IRIS_PHYSX_VISIBILITY_TESTER_
