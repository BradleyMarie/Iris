/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    visibility_tester.h

Abstract:

    Allows testing for visibility along a ray. A visibility test is considered
    successful if there are no successful hit tests along the ray inside the
    distance tested.

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
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ float_t distance_to_object,
    _Out_ bool *visible
    );

ISTATUS
VisibilityTesterTestAnyDistance(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _Out_ bool *visible
    );

#endif // _IRIS_PHYSX_VISIBILITY_TESTER_
