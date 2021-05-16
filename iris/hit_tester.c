/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    hit_tester.c

Abstract:

    Intersection tester.

--*/

#include "iris/hit_tester.h"

#include "iris/hit_tester_internal.h"
#include "iris/multiply_internal.h"

//
// Static Functions
//

static
inline
ISTATUS
HitTesterTestGeometryInternal(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied,
    _Out_ float_t *maybe_farthest_hit_allowed
    )
{
    if (hit_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (test_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PRAY trace_ray;
    RAY ray_storage;
    if (model_to_world == NULL || premultiplied)
    {
        trace_ray = &hit_tester->world_ray;
    }
    else
    {
        ray_storage = RayMatrixInverseMultiplyInline(model_to_world,
                                                     hit_tester->world_ray);
        trace_ray = &ray_storage;
    }

    HitAllocatorSetRay(&hit_tester->hit_allocator, trace_ray);
    HitAllocatorSetData(&hit_tester->hit_allocator, hit_data);

    PHIT hit;
    ISTATUS status = test_routine(geometry_data,
                                  trace_ray,
                                  &hit_tester->hit_allocator,
                                  &hit);

    if (status == ISTATUS_NO_INTERSECTION)
    {
        return ISTATUS_SUCCESS;
    }

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    do
    {
        if (hit_tester->minimum_distance <= hit->distance &&
            hit->distance <= hit_tester->maximum_distance)
        {
            PFULL_HIT_CONTEXT full_hit_context = (PFULL_HIT_CONTEXT)(void *)hit;
            full_hit_context->model_to_world = model_to_world;
            full_hit_context->premultiplied = premultiplied;
            hit_tester->closest_hit = full_hit_context;
            hit_tester->maximum_distance = hit->distance;

            if (maybe_farthest_hit_allowed != NULL)
            {
                *maybe_farthest_hit_allowed = hit->distance;
            }
        }

        hit = hit->next;
    } while (hit != NULL);

    HitAllocatorFreeAllExcept(&hit_tester->hit_allocator,
                              hit_tester->closest_hit->allocation_handle);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
HitTesterFarthestHitAllowed(
    _In_ PCHIT_TESTER hit_tester,
    _Out_ float_t *distance
    )
{
    if (hit_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (distance == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *distance = hit_tester->maximum_distance;

    return ISTATUS_SUCCESS;
}

ISTATUS
HitTesterTestWorldGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   NULL,
                                                   false,
                                                   NULL);

    return status;
}

ISTATUS
HitTesterTestPremultipliedGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   true,
                                                   NULL);

    return status;
}

ISTATUS
HitTesterTestTransformedGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   false,
                                                   NULL);

    return status;
}

ISTATUS
HitTesterTestGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   premultiplied,
                                                   NULL);

    return status;
}

ISTATUS
HitTesterTestWorldGeometryWithLimit(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _Out_opt_ float_t *maybe_farthest_hit_allowed
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   NULL,
                                                   false,
                                                   maybe_farthest_hit_allowed);

    return status;
}

ISTATUS
HitTesterTestPremultipliedGeometryWithLimit(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _Out_opt_ float_t *maybe_farthest_hit_allowed
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   true,
                                                   maybe_farthest_hit_allowed);

    return status;
}

ISTATUS
HitTesterTestTransformedGeometryWithLimit(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _Out_opt_ float_t *maybe_farthest_hit_allowed
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   false,
                                                   maybe_farthest_hit_allowed);

    return status;
}

ISTATUS
HitTesterTestGeometryWithLimit(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied,
    _Out_opt_ float_t *maybe_farthest_hit_allowed
    )
{
    ISTATUS status = HitTesterTestGeometryInternal(hit_tester,
                                                   test_routine,
                                                   geometry_data,
                                                   hit_data,
                                                   model_to_world,
                                                   premultiplied,
                                                   maybe_farthest_hit_allowed);

    return status;
}

ISTATUS
HitTesterTestNestedGeometry(
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _Out_ PHIT *hits
    )
{
    if (hit_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (test_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (hits == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PCRAY model_ray = HitAllocatorGetRay(hit_allocator);

    const void *original_data = HitAllocatorGetData(hit_allocator);
    HitAllocatorSetData(hit_allocator, hit_data);

    ISTATUS status = test_routine(geometry_data,
                                  model_ray,
                                  hit_allocator,
                                  hits);

    HitAllocatorSetData(hit_allocator, original_data);

    return status;
}