/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    hit_tester.h

Abstract:

    Tests if an object intersects with a ray. The ray being intersected is 
    not listed as an argument to any functions in this file because the 
    ray is implicity passed down from the ray tracer.

    There are three possible ways of checking for an intersection.

    1) Test against an object known to reside in world coordinates. For these 
       objects use HitTesterTestWorldGeometry.

    2) Test against an object which defines a coordinate space, but whose 
       intersection test may be done in the world coordinate space. For these
       objects use HitTesterTestPremultipliedGeometry.
    
    3) Test against an object which resides in its own coordinate space and 
       which must have its intersection test done in its model coordinate space.
       for these objects use HitTesterTestGeometry.
    
    Note, the right thing will happen if the transformation passed in is the 
    identity matrix.

    There is also one other, related form of hit testing. If an object is nested
    within another object, HitTesterTestNestedGeometry allows for intersection
    testing against the second level object. Iris only supports nested objects
    which reside in the same coordinate space as the top level object. Unlike
    the other hit test routines here, HitTesterTestNestedGeometry returns back
    the list of hits immediately, so it is the responsibility of its caller to
    propigate those hits back up the call stack.

    These routines also allow the data associated with the hits to be different
    than the data used during intersection testing. This may be useful if the 
    data needed to complete an intersection test is nested within some auxiliary
    structure.

--*/

#ifndef _IRIS_HIT_TESTER_
#define _IRIS_HIT_TESTER_

#include "iris/hit_allocator.h"
#include "iris/matrix.h"
#include "iris/ray.h"

//
// Types
//

typedef struct _HIT_TESTER HIT_TESTER, *PHIT_TESTER;
typedef const HIT_TESTER *PCHIT_TESTER;

typedef
ISTATUS 
(*PHIT_TESTER_TEST_GEOMETRY_ROUTINE)(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Outptr_result_maybenull_ PHIT *hits
    );

//
// Functions
//

ISTATUS
HitTesterTestGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data
    );

ISTATUS
HitTesterTestPremultipliedGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world
    );

ISTATUS
HitTesterTestTransformedGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world
    );

ISTATUS
HitTesterTestNestedGeometry(
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _Outptr_result_maybenull_ PHIT *hits
    );

#endif // _IRIS_HIT_TESTER_