/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    hit_tester.h

Abstract:

    Hit tester for Iris Physx intersection tests. See documentation in 
    "iris/hit_tester.h" for further detail.

--*/

#ifndef _IRIS_PHYSX_HIT_TESTER_
#define _IRIS_PHYSX_HIT_TESTER_

#include "iris_physx/hit_allocator.h"
#include "iris_physx/shape.h"

//
// Types
//

typedef HIT_TESTER SHAPE_HIT_TESTER, *PSHAPE_HIT_TESTER;
typedef const SHAPE_HIT_TESTER *PCSHAPE_HIT_TESTER;

//
// Functions
//

static
inline
ISTATUS
ShapeHitTesterTestWorldShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape
    )
{
    if (shape == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestWorldGeometry(hit_tester,
                                                test_routine,
                                                context,
                                                shape);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestPremultipliedShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world
    )
{
    if (shape == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestPremultipliedGeometry(hit_tester,
                                                        test_routine,
                                                        context,
                                                        shape,
                                                        model_to_world);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestTransformedShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world
    )
{
    if (shape == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestTransformedGeometry(hit_tester,
                                                      test_routine,
                                                      context,
                                                      shape,
                                                      model_to_world);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestShape(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied
    )
{
    if (shape == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestGeometry(hit_tester,
                                           test_routine,
                                           context,
                                           shape,
                                           model_to_world,
                                           premultiplied);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestNestedShape(
    _Inout_ PSHAPE_HIT_ALLOCATOR hit_allocator,
    _In_ PCSHAPE shape,
    _Out_ PHIT *hits
    )
{
    if (hits == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (shape == NULL)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestNestedGeometry(hit_allocator,
                                                 test_routine,
                                                 context,
                                                 shape,
                                                 hits);

    return status;
}

#endif // _IRIS_PHYSX_HIT_TESTER_