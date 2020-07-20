/*++

Copyright (c) 2020 Brad Weinberger

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
ShapeHitTesterFarthestHitAllowed(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _Out_ float_t *distance
    )
{
    return HitTesterFarthestHitAllowed(hit_tester, distance);
}

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
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
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
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
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
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
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
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
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
ShapeHitTesterTestWorldShapeWithLimit(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _Out_opt_ float_t *farthest_hit_allowed
    )
{
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
    ISTATUS status = HitTesterTestWorldGeometryWithLimit(hit_tester,
                                                         test_routine,
                                                         context,
                                                         shape,
                                                         farthest_hit_allowed);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestPremultipliedShapeWithLimit(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world,
    _Out_opt_ float_t *farthest_hit_allowed
    )
{
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
    ISTATUS status =
        HitTesterTestPremultipliedGeometryWithLimit(hit_tester,
                                                    test_routine,
                                                    context,
                                                    shape,
                                                    model_to_world,
                                                    farthest_hit_allowed);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestTransformedShapeWithLimit(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world,
    _Out_opt_ float_t *farthest_hit_allowed
    )
{
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
    ISTATUS status =
        HitTesterTestTransformedGeometryWithLimit(hit_tester,
                                                  test_routine,
                                                  context,
                                                  shape,
                                                  model_to_world,
                                                  farthest_hit_allowed);

    return status;
}

static
inline
ISTATUS
ShapeHitTesterTestShapeWithLimit(
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ PCSHAPE shape,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied,
    _Out_opt_ float_t *farthest_hit_allowed
    )
{
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
    ISTATUS status = HitTesterTestGeometryWithLimit(hit_tester,
                                                    test_routine,
                                                    context,
                                                    shape,
                                                    model_to_world,
                                                    premultiplied,
                                                    farthest_hit_allowed);

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
    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (hits == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *context = (const void*)((const void **)shape + 2);
    ISTATUS status = HitTesterTestNestedGeometry(hit_allocator,
                                                 test_routine,
                                                 context,
                                                 shape,
                                                 hits);

    return status;
}

#endif // _IRIS_PHYSX_HIT_TESTER_