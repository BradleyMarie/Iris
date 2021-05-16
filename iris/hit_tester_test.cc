/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    hit_tester_test.cc

Abstract:

    Unit tests for the shared hit tester

--*/

extern "C" {
#include "iris/hit_tester.h"
#include "iris/hit_tester_internal.h"
#include "iris/multiply.h"
}

#include <random>
#include <map>
#include <set>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

struct NestedParams {
    const void *data;
    RAY ray;
    ISTATUS status_to_return;
    bool *triggered;
    std::vector<float> distances;
    size_t elements_per_node;
    size_t *nest_count;
};

ISTATUS 
NestedCheckGeometryContext(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Out_ PHIT *hits
    )
{
    const NestedParams* param = static_cast<const NestedParams *>(data);
    EXPECT_EQ(param->data, data);
    EXPECT_EQ(param->ray, *ray);
    *param->triggered = true;

    *hits = NULL;
    if (!param->distances.empty())
    {
        size_t current_depth = *param->nest_count;
        *param->nest_count += 1;

        if (current_depth * param->elements_per_node < param->distances.size())
        {
            PHIT next_hit;
            ISTATUS status = HitTesterTestNestedGeometry(hit_allocator,
                                                         NestedCheckGeometryContext,
                                                         data,
                                                         nullptr,
                                                         &next_hit);
            if (param->status_to_return != ISTATUS_SUCCESS)
            {
                EXPECT_EQ(param->status_to_return, status);
            }

            for (size_t i = 0; i < param->elements_per_node; i++) {
                size_t element = current_depth * param->elements_per_node + i;
                status = HitAllocatorAllocate(hit_allocator,
                                              next_hit,
                                              param->distances.at(element),
                                              1,
                                              2,
                                              nullptr,
                                              0,
                                              0,
                                              &next_hit);
                EXPECT_EQ(ISTATUS_SUCCESS, status);
            }

            *hits = next_hit;
        }
    }

    if (*hits == NULL && param->status_to_return == ISTATUS_SUCCESS)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    return param->status_to_return;
}

struct ValidationParams {
    const void *data;
    RAY ray;
    ISTATUS status_to_return;
    bool *triggered;
};

ISTATUS 
CheckGeometryContext(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Out_ PHIT *hits
    )
{
    const ValidationParams* param = static_cast<const ValidationParams *>(data);
    EXPECT_EQ(param->data, data);
    EXPECT_EQ(param->ray, *ray);
    EXPECT_TRUE(hit_allocator);

    *param->triggered = true;

    bool triggered = false;
    NestedParams nested_params;
    nested_params.data = &nested_params;
    nested_params.ray = param->ray;
    nested_params.triggered = &triggered;

    PHIT nested_hits;
    nested_params.status_to_return = ISTATUS_NO_INTERSECTION;
    ISTATUS status = HitTesterTestNestedGeometry(hit_allocator,
                                                 NestedCheckGeometryContext,
                                                 &nested_params,
                                                 nullptr,
                                                 &nested_hits);
    EXPECT_EQ(ISTATUS_NO_INTERSECTION, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    nested_params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_30;
    status = HitTesterTestNestedGeometry(hit_allocator,
                                         NestedCheckGeometryContext,
                                         &nested_params,
                                         nullptr,
                                         &nested_hits);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_30, status);
    EXPECT_TRUE(triggered);

    size_t nest_count = 0;
    nested_params.nest_count = &nest_count;
    nested_params.elements_per_node = 2;
    nested_params.distances = { 1.0f, 0.0f, 3.0f, 2.0f, 5.0f, 4.0f };

    nested_params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestNestedGeometry(hit_allocator,
                                         NestedCheckGeometryContext,
                                         &nested_params,
                                         nullptr,
                                         &nested_hits);
    EXPECT_EQ(nested_params.status_to_return, status);
    EXPECT_EQ(4u, nest_count);
    
    float number = 0.0f;
    int num_hits = 0;
    while (nested_hits != NULL)
    {
        EXPECT_EQ(number, nested_hits->distance);
        num_hits += 1;
        number += 1.0f;
        nested_hits = nested_hits->next;
    }
    EXPECT_EQ(6, num_hits);

    if (param->status_to_return == ISTATUS_SUCCESS)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    return param->status_to_return;
}

TEST(HitTesterTest, HitTesterTestWorldGeometryArgumentErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestWorldGeometry(nullptr,
                                                CheckGeometryContext,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestWorldGeometry(&tester,
                                        nullptr,
                                        nullptr,
                                        nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = HitTesterTestWorldGeometryWithLimit(nullptr,
                                                 CheckGeometryContext,
                                                 nullptr,
                                                 nullptr,
                                                 nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestWorldGeometryWithLimit(&tester,
                                                 nullptr,
                                                 nullptr,
                                                 nullptr,
                                                 nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestWorldGeometry)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));
    
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    bool triggered = false;
    ValidationParams params;
    params.ray = ray;
    params.data = &params;
    params.status_to_return = ISTATUS_SUCCESS;
    params.triggered = &triggered;

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    ISTATUS status = HitTesterTestWorldGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;
    status = HitTesterTestWorldGeometry(&tester,
                                        CheckGeometryContext,
                                        &params,
                                        nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestWorldGeometry(&tester,
                                        CheckGeometryContext,
                                        &params,
                                        nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestPremultipliedGeometryArgumentErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestPremultipliedGeometry(nullptr,
                                                        CheckGeometryContext,
                                                        nullptr,
                                                        nullptr,
                                                        nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestPremultipliedGeometry(&tester,
                                                nullptr,
                                                nullptr,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = HitTesterTestPremultipliedGeometryWithLimit(nullptr,
                                                         CheckGeometryContext,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestPremultipliedGeometryWithLimit(&tester,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);   
}

TEST(HitTesterTest, HitTesterTestPremultipliedGeometry)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));
    
    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    bool triggered = false;
    ValidationParams params;
    params.ray = ray;
    params.data = &params;
    params.status_to_return = ISTATUS_SUCCESS;
    params.triggered = &triggered;

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                model_to_world);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                model_to_world);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;

    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                model_to_world);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(params.triggered);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
}

TEST(HitTesterTest, HitTesterTestTransformedGeometryArgumentErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestTransformedGeometry(nullptr,
                                                      CheckGeometryContext,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestTransformedGeometry(&tester,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = HitTesterTestTransformedGeometryWithLimit(nullptr,
                                                       CheckGeometryContext,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestTransformedGeometryWithLimit(&tester,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestTransformedGeometry)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    bool triggered = false;
    ValidationParams params;
    params.ray = ray;
    params.data = &params;
    params.status_to_return = ISTATUS_SUCCESS;
    params.triggered = &triggered;

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;

    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(params.triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;

    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(params.triggered);

    POINT3 model_origin = PointCreate((float_t) 0.0, 
                                      (float_t) 0.0, 
                                      (float_t) 0.0);
    VECTOR3 model_direction = VectorCreate((float_t) 4.0,
                                           (float_t) 5.0,
                                           (float_t) 6.0);
    params.ray = RayCreate(model_origin, model_direction);

    triggered = false;
    params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              model_to_world);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;
    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              model_to_world);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              model_to_world);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
}

ISTATUS 
AllocateHitAtDistance(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Out_ PHIT *hits
    )
{
    const float_t* distance = (const float_t*)data;
    ISTATUS status = HitAllocatorAllocate(hit_allocator,
                                          NULL,
                                          *distance,
                                          1,
                                          2,
                                          nullptr,
                                          0,
                                          0,
                                          hits);
    return status;
}

TEST(HitTesterTest, HitTesterSortHits)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);

    int hit_data = 0;
    for (int i = 1; i <= 1000; i++)
    {
        float_t distance = (float_t)(1001 - i);
        ISTATUS status = HitTesterTestWorldGeometry(&tester,
                                                    AllocateHitAtDistance,
                                                    &distance,
                                                    &hit_data);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
    }

    float_t closest_hit;
    ISTATUS status = HitTesterFarthestHitAllowed(&tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, closest_hit);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterRejectHits)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    HitTesterReset(&tester, ray, (float_t)10.0, (float_t)20.0);

    int hit_data = 0;
    for (int i = 1; i <= 1000; i++)
    {
        float_t distance = (float_t)(1001 - i);
        ISTATUS status = HitTesterTestWorldGeometry(&tester,
                                                    AllocateHitAtDistance,
                                                    &distance,
                                                    &hit_data);
        EXPECT_EQ(ISTATUS_SUCCESS, status);

        if ((float_t)10.0 <= distance && distance <= (float_t)20.0)
        {
            float_t closest_hit;
            status = HitTesterFarthestHitAllowed(&tester, &closest_hit);
            EXPECT_EQ(distance, closest_hit);
        }
    }

    float_t closest_hit;
    ISTATUS status = HitTesterFarthestHitAllowed(&tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)10.0, closest_hit);

    HitTesterDestroy(&tester);
}

ISTATUS 
AllocateTwoHitAtDistance(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Out_ PHIT *hits
    )
{
    const float_t* distance = (const float_t*)data;
    ISTATUS status = HitAllocatorAllocate(hit_allocator,
                                          NULL,
                                          *distance + (float_t)1,
                                          1,
                                          2,
                                          nullptr,
                                          0,
                                          0,
                                          hits);

    status = HitAllocatorAllocate(hit_allocator,
                                  *hits,
                                  *distance,
                                  1,
                                  2,
                                  nullptr,
                                  0,
                                  0,
                                  hits);
    return status;
}

ISTATUS 
AllocateNoHits(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Out_ PHIT *hits
    )
{
    return ISTATUS_NO_INTERSECTION;
}

void RunWorldHitTest(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance
    )
{
    float_t farthest_hit_allowed = INFINITY;
    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestWorldGeometryWithLimit(tester,
                                                         AllocateTwoHitAtDistance,
                                                         &first_distance,
                                                         &first_hit_data,
                                                         &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    float_t closest_hit;
    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(first_distance, closest_hit);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestWorldGeometry(tester,
                                        AllocateTwoHitAtDistance,
                                        &second_distance,
                                        &second_hit_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestWorldGeometryWithLimit(tester,
                                                 AllocateTwoHitAtDistance,
                                                 &third_distance,
                                                 &third_hit_data,
                                                 nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestWorldGeometryWithLimit(tester,
                                                 AllocateHitAtDistance,
                                                 &fourth_distance,
                                                 &fourth_hit_data,
                                                 &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    status = HitTesterTestWorldGeometry(tester,
                                        AllocateNoHits,
                                        nullptr,
                                        nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                &third_hit_data == tester->closest_hit->context.data);
    EXPECT_EQ(NULL, tester->closest_hit->model_to_world);
}

TEST(HitTesterTest, HitTesterCheckWorldHits)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunWorldHitTest(&tester, (float_t)0.0);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunWorldHitTest(&tester, (float_t)0.0);
    
    HitTesterDestroy(&tester);
}

void RunPremultipliedHitTest(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance,
    _In_ PCMATRIX matrix
    )
{
    float_t farthest_hit_allowed = INFINITY;
    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestPremultipliedGeometryWithLimit(tester,
                                                                 AllocateTwoHitAtDistance,
                                                                 &first_distance,
                                                                 &first_hit_data,
                                                                 matrix,
                                                                 &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    float_t closest_hit;
    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(first_distance, closest_hit);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateTwoHitAtDistance,
                                                &second_distance,
                                                &second_hit_data,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestPremultipliedGeometryWithLimit(tester,
                                                         AllocateTwoHitAtDistance,
                                                         &third_distance,
                                                         &third_hit_data,
                                                         matrix,
                                                         nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestPremultipliedGeometryWithLimit(tester,
                                                         AllocateHitAtDistance,
                                                         &fourth_distance,
                                                         &fourth_hit_data,
                                                         matrix,
                                                         &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateNoHits,
                                                nullptr,
                                                nullptr,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    if (matrix != nullptr)
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_TRUE(tester->closest_hit->premultiplied);
        EXPECT_EQ(matrix, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
    else
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_EQ(NULL, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
}

TEST(HitTesterTest, HitTesterCheckPremultipliedHits)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX model_to_world_2;
    status = MatrixAllocateTranslation((float_t) 1.0,
                                       (float_t) 2.0,
                                       (float_t) 3.0,
                                       &model_to_world_2);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTest(&tester, (float_t)0.0, nullptr);
    
    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}

void RunTransformedHitTest(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance,
    _In_ PCMATRIX matrix
    )
{
    float_t farthest_hit_allowed = INFINITY;
    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestTransformedGeometryWithLimit(tester,
                                                               AllocateTwoHitAtDistance,
                                                               &first_distance,
                                                               &first_hit_data,
                                                               matrix,
                                                               &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    float_t closest_hit;
    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(first_distance, closest_hit);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateTwoHitAtDistance,
                                              &second_distance,
                                              &second_hit_data,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestTransformedGeometryWithLimit(tester,
                                                       AllocateTwoHitAtDistance,
                                                       &third_distance,
                                                       &third_hit_data,
                                                       matrix,
                                                       nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestTransformedGeometryWithLimit(tester,
                                                       AllocateHitAtDistance,
                                                       &fourth_distance,
                                                       &fourth_hit_data,
                                                       matrix,
                                                       &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateNoHits,
                                              nullptr,
                                              nullptr,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    if (matrix == nullptr)
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_EQ(NULL, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
    else
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_FALSE(tester->closest_hit->premultiplied);
        EXPECT_EQ(matrix, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
}

TEST(HitTesterTest, HitTesterCheckTransformedHits)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX model_to_world_2;
    status = MatrixAllocateTranslation((float_t) 1.0,
                                       (float_t) 2.0,
                                       (float_t) 3.0,
                                       &model_to_world_2);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTest(&tester, 0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}

TEST(HitTesterTest, HitTesterTestGeometryArgumentErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestGeometry(nullptr,
                                           CheckGeometryContext,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           false);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestGeometry(&tester,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   false);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = HitTesterTestGeometryWithLimit(nullptr,
                                            CheckGeometryContext,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            false,
                                            nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestGeometryWithLimit(&tester,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            false,
                                            nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestGeometryErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    bool triggered = false;
    ValidationParams params;
    params.ray = ray;
    params.data = &params;
    params.status_to_return = ISTATUS_SUCCESS;
    params.triggered = &triggered;

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;

    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(params.triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;

    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   false);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(params.triggered);

    POINT3 model_origin = PointCreate((float_t) 0.0,
                                      (float_t) 0.0,
                                      (float_t) 0.0);
    VECTOR3 model_direction = VectorCreate((float_t) 4.0,
                                           (float_t) 5.0,
                                           (float_t) 6.0);
    params.ray = RayCreate(model_origin, model_direction);

    triggered = false;
    params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   model_to_world,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_INTERSECTION;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   model_to_world,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   model_to_world,
                                   false);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
}

void
RunPremultipliedHitTestBase(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance,
    _In_ PCMATRIX matrix
    )
{
    float_t farthest_hit_allowed = INFINITY;
    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestGeometryWithLimit(tester,
                                                    AllocateTwoHitAtDistance,
                                                    &first_distance,
                                                    &first_hit_data,
                                                    matrix,
                                                    true,
                                                    &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    float_t closest_hit;
    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(first_distance, closest_hit);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestGeometry(tester,
                                   AllocateTwoHitAtDistance,
                                   &second_distance,
                                   &second_hit_data,
                                   matrix,
                                   true);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestGeometryWithLimit(tester,
                                            AllocateTwoHitAtDistance,
                                            &third_distance,
                                            &third_hit_data,
                                            matrix,
                                            true,
                                            nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestGeometryWithLimit(tester,
                                            AllocateHitAtDistance,
                                            &fourth_distance,
                                            &fourth_hit_data,
                                            matrix,
                                            true,
                                            &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    status = HitTesterTestGeometry(tester,
                                   AllocateNoHits,
                                   nullptr,
                                   nullptr,
                                   matrix,
                                   true);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    if (matrix != nullptr)
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_TRUE(tester->closest_hit->premultiplied);
        EXPECT_EQ(matrix, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
    else
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_EQ(NULL, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
}

TEST(HitTesterTest, HitTesterCheckGeometryHitsPremultiplied)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX model_to_world_2;
    status = MatrixAllocateTranslation((float_t) 1.0,
                                       (float_t) 2.0,
                                       (float_t) 3.0,
                                       &model_to_world_2);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunPremultipliedHitTestBase(&tester, (float_t)0.0, nullptr);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}

void
RunTransformedHitTestBase(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance,
    _In_ PCMATRIX matrix
    )
{
    float_t farthest_hit_allowed = INFINITY;
    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestGeometryWithLimit(tester,
                                                    AllocateTwoHitAtDistance,
                                                    &first_distance,
                                                    &first_hit_data,
                                                    matrix,
                                                    false,
                                                    &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    float_t closest_hit;
    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(first_distance, closest_hit);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestGeometry(tester,
                                   AllocateTwoHitAtDistance,
                                   &second_distance,
                                   &second_hit_data,
                                   matrix,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestGeometryWithLimit(tester,
                                            AllocateTwoHitAtDistance,
                                            &third_distance,
                                            &third_hit_data,
                                            matrix,
                                            false,
                                            nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestGeometryWithLimit(tester,
                                            AllocateHitAtDistance,
                                            &fourth_distance,
                                            &fourth_hit_data,
                                            matrix,
                                            false,
                                            &farthest_hit_allowed);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, farthest_hit_allowed);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    status = HitTesterTestGeometry(tester,
                                   AllocateNoHits,
                                   nullptr,
                                   nullptr,
                                   matrix,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    status = HitTesterFarthestHitAllowed(tester, &closest_hit);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(second_distance, closest_hit);

    if (matrix == nullptr)
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_EQ(NULL, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
    else
    {
        EXPECT_TRUE(&second_hit_data == tester->closest_hit->context.data ||
                    &third_hit_data == tester->closest_hit->context.data);
        EXPECT_FALSE(tester->closest_hit->premultiplied);
        EXPECT_EQ(matrix, tester->closest_hit->model_to_world);

        status = HitTesterFarthestHitAllowed(tester, &closest_hit);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(second_distance, closest_hit);
    }
}

TEST(HitTesterTest, HitTesterCheckGeometryHitsTransformed)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PMATRIX model_to_world_2;
    status = MatrixAllocateTranslation((float_t) 1.0,
                                       (float_t) 2.0,
                                       (float_t) 3.0,
                                       &model_to_world_2);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, model_to_world);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, model_to_world_2);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    RunTransformedHitTestBase(&tester, 0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0, INFINITY);
    EXPECT_EQ(INFINITY, tester.closest_hit->hit.distance);

    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}

TEST(HitTesterTest, HitTesterFarthestHitAllowedArgumentErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    float_t closest_hit;
    ISTATUS status = HitTesterFarthestHitAllowed(nullptr, &closest_hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterFarthestHitAllowed(&tester, nullptr);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterClosestEmpty)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    float_t closest_hit;
    ISTATUS status = HitTesterFarthestHitAllowed(&tester, &closest_hit);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_TRUE(std::isinf(closest_hit));

    HitTesterDestroy(&tester);
}