/*++

Copyright (c) 2019 Brad Weinberger

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
    _Outptr_result_maybenull_ PHIT *hits
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
            EXPECT_EQ(param->status_to_return, status);

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
    _Outptr_result_maybenull_ PHIT *hits
    )
{
    const ValidationParams* param = static_cast<const ValidationParams *>(data);
    EXPECT_EQ(param->data, data);
    EXPECT_EQ(param->ray, *ray);
    EXPECT_TRUE(hit_allocator);
    *hits = NULL;

    *param->triggered = true;

    bool triggered = false;
    NestedParams nested_params;
    nested_params.data = &nested_params;
    nested_params.ray = param->ray;
    nested_params.triggered = &triggered;

    PHIT hits;
    nested_params.status_to_return = ISTATUS_SUCCESS;
    ISTATUS status = HitTesterTestNestedGeometry(hit_allocator,
                                                 NestedCheckGeometryContext,
                                                 &nested_params,
                                                 nullptr,
                                                 &hits);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(nullptr, hits);
    EXPECT_TRUE(triggered);

    triggered = false;
    nested_params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_30;
    status = HitTesterTestNestedGeometry(hit_allocator,
                                         NestedCheckGeometryContext,
                                         &nested_params,
                                         nullptr,
                                         &hits);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_30, status);
    EXPECT_EQ(nullptr, hits);
    EXPECT_TRUE(triggered);

    size_t nest_count = 0;
    nested_params.nest_count = &nest_count;
    nested_params.elements_per_node = 2;
    nested_params.distances = { 1.0f, 0.0f, 3.0f, 2.0f, 5.0f, 4.0f };

    PHIT nested_hits;
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

    return param->status_to_return;
}

TEST(HitTesterTest, HitTesterTestWorldGeometryErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestGeometry(nullptr,
                                           CheckGeometryContext,
                                           nullptr,
                                           nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestGeometry(&tester,
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

    HitTesterReset(&tester, ray, (float_t)0.0);
    ISTATUS status = HitTesterTestGeometry(&tester,
                                           CheckGeometryContext,
                                           &params,
                                           nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_RESULT;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestPremultipliedGeometryErrors)
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

    HitTesterReset(&tester, ray, (float_t)0.0);
    status = HitTesterTestPremultipliedGeometry(&tester,
                                                CheckGeometryContext,
                                                &params,
                                                nullptr,
                                                nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_RESULT;
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
    params.status_to_return = ISTATUS_NO_RESULT;
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

TEST(HitTesterTest, HitTesterTestTransformedGeometryErrors)
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

    HitTesterReset(&tester, ray, (float_t)0.0);
    status = HitTesterTestTransformedGeometry(&tester,
                                              CheckGeometryContext,
                                              &params,
                                              nullptr,
                                              nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_NO_RESULT;

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
    params.status_to_return = ISTATUS_NO_RESULT;
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
    _Outptr_result_maybenull_ PHIT *hits
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

    HitTesterReset(&tester, ray, (float_t)0.0);

    int hit_data = 0;
    for (int i = 1; i <= 1000; i++)
    {
        float_t distance = (float_t)(1001 - i);
        ISTATUS status = HitTesterTestGeometry(&tester,
                                               AllocateHitAtDistance,
                                               &distance,
                                               &hit_data);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
    }

    size_t size;
    PCFULL_HIT_CONTEXT* hits;
    HitTesterGetHits(&tester, &hits, &size);
    ASSERT_EQ(1000u, size);

    for (int i = 0; i < 1000; i++)
    {
        EXPECT_EQ((float_t)(1000 - i), hits[i]->context.distance);
    }

    HitTesterSortHits(&tester);

    for (int i = 0; i < 1000; i++)
    {
        EXPECT_EQ((float_t)(i + 1), hits[i]->context.distance);
    }

    HitTesterDestroy(&tester);
}

ISTATUS 
AllocateTwoHitAtDistance(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Outptr_result_maybenull_ PHIT *hits
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
    _Outptr_result_maybenull_ PHIT *hits
    )
{
    *hits = NULL;
    return ISTATUS_SUCCESS;
}

void RunWorldHitTest(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance
    )
{
    PCFULL_HIT_CONTEXT *hits;
    size_t offset;
    HitTesterGetHits(tester, &hits, &offset);

    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestGeometry(tester,
                                           AllocateTwoHitAtDistance,
                                           &first_distance,
                                           &first_hit_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    size_t num_hits;
    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(2u + offset, num_hits);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestGeometry(tester,
                                   AllocateTwoHitAtDistance,
                                   &second_distance,
                                   &second_hit_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(4u + offset, num_hits);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestGeometry(tester,
                                   AllocateTwoHitAtDistance,
                                   &third_distance,
                                   &third_hit_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(5u + offset, num_hits);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestGeometry(tester,
                                   AllocateHitAtDistance,
                                   &fourth_distance,
                                   &fourth_hit_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(6u + offset, num_hits);

    status = HitTesterTestGeometry(tester,
                                   AllocateNoHits,
                                   nullptr,
                                   nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    ASSERT_EQ(6u + offset, num_hits);

    // First
    EXPECT_EQ(&first_hit_data, hits[0 + offset]->context.data);
    EXPECT_EQ(NULL, hits[0 + offset]->shared_context);
    EXPECT_EQ(first_distance, hits[0 + offset]->context.distance);
    EXPECT_EQ(&first_hit_data, hits[1 + offset]->context.data);
    EXPECT_EQ(NULL, hits[1 + offset]->shared_context);
    EXPECT_EQ(first_distance + 1.0, hits[1 + offset]->context.distance);

    // Second
    EXPECT_EQ(&second_hit_data, hits[2 + offset]->context.data);
    EXPECT_EQ(NULL, hits[2 + offset]->shared_context);
    EXPECT_EQ(second_distance, hits[2 + offset]->context.distance);
    EXPECT_EQ(&second_hit_data, hits[3 + offset]->context.data);
    EXPECT_EQ(NULL, hits[3 + offset]->shared_context);
    EXPECT_EQ(second_distance + 1.0, hits[3 + offset]->context.distance);

    // Third
    EXPECT_EQ(&third_hit_data, hits[4 + offset]->context.data);
    EXPECT_EQ(NULL, hits[4 + offset]->shared_context);
    EXPECT_EQ(third_distance + 1.0, hits[4 + offset]->context.distance);

    // Fourth
    EXPECT_EQ(&fourth_hit_data, hits[5 + offset]->context.data);
    EXPECT_EQ(NULL, hits[5 + offset]->shared_context);
    EXPECT_EQ(fourth_distance, hits[5 + offset]->context.distance);
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

    HitTesterReset(&tester, ray, (float_t)0.0);

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunWorldHitTest(&tester, (float_t)0.0);

    HitTesterReset(&tester, ray, (float_t)0.0);
    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunWorldHitTest(&tester, (float_t)0.0);
    
    HitTesterDestroy(&tester);
}

void RunPremultipliedHitTest(
    _In_ PHIT_TESTER tester,
    _In_ float_t min_distance,
    _In_ PCMATRIX matrix
    )
{
    PCFULL_HIT_CONTEXT *hits;
    size_t offset;
    HitTesterGetHits(tester, &hits, &offset);

    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestPremultipliedGeometry(tester,
                                                        AllocateTwoHitAtDistance,
                                                        &first_distance,
                                                        &first_hit_data,
                                                        matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    size_t num_hits;
    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(2u + offset, num_hits);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateTwoHitAtDistance,
                                                &second_distance,
                                                &second_hit_data,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(4u + offset, num_hits);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateTwoHitAtDistance,
                                                &third_distance,
                                                &third_hit_data,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(5u + offset, num_hits);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateHitAtDistance,
                                                &fourth_distance,
                                                &fourth_hit_data,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(6u + offset, num_hits);

    status = HitTesterTestPremultipliedGeometry(tester,
                                                AllocateNoHits,
                                                nullptr,
                                                nullptr,
                                                matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    ASSERT_EQ(6u + offset, num_hits);

    if (matrix != nullptr)
    {
        // First
        EXPECT_EQ(&first_hit_data, hits[0 + offset]->context.data);
        EXPECT_TRUE(hits[0 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[0 + offset]->shared_context->model_to_world);
        EXPECT_EQ(first_distance, hits[0 + offset]->context.distance);

        EXPECT_EQ(&first_hit_data, hits[1 + offset]->context.data);
        EXPECT_TRUE(hits[1 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[1 + offset]->shared_context->model_to_world);
        EXPECT_EQ(first_distance + 1.0, hits[1 + offset]->context.distance);

        // Second
        EXPECT_EQ(&second_hit_data, hits[2 + offset]->context.data);
        EXPECT_TRUE(hits[2 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[2 + offset]->shared_context->model_to_world);
        EXPECT_EQ(second_distance, hits[2 + offset]->context.distance);

        EXPECT_EQ(&second_hit_data, hits[3 + offset]->context.data);
        EXPECT_TRUE(hits[3 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[3 + offset]->shared_context->model_to_world);
        EXPECT_EQ(second_distance + 1.0, hits[3 + offset]->context.distance);

        // Third
        EXPECT_EQ(&third_hit_data, hits[4 + offset]->context.data);
        EXPECT_TRUE(hits[4 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[4 + offset]->shared_context->model_to_world);
        EXPECT_EQ(third_distance + 1.0, hits[4 + offset]->context.distance);

        // Fourth
        EXPECT_EQ(&fourth_hit_data, hits[5 + offset]->context.data);
        EXPECT_TRUE(hits[5 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[5 + offset]->shared_context->model_to_world);
        EXPECT_EQ(fourth_distance, hits[5 + offset]->context.distance);
    }
    else
    {
        // First
        EXPECT_EQ(&first_hit_data, hits[0 + offset]->context.data);
        EXPECT_EQ(NULL, hits[0 + offset]->shared_context);
        EXPECT_EQ(first_distance, hits[0 + offset]->context.distance);
        EXPECT_EQ(&first_hit_data, hits[1 + offset]->context.data);
        EXPECT_EQ(NULL, hits[1 + offset]->shared_context);
        EXPECT_EQ(first_distance + 1.0, hits[1 + offset]->context.distance);

        // Second
        EXPECT_EQ(&second_hit_data, hits[2 + offset]->context.data);
        EXPECT_EQ(NULL, hits[2 + offset]->shared_context);
        EXPECT_EQ(second_distance, hits[2 + offset]->context.distance);
        EXPECT_EQ(&second_hit_data, hits[3 + offset]->context.data);
        EXPECT_EQ(NULL, hits[3 + offset]->shared_context);
        EXPECT_EQ(second_distance + 1.0, hits[3 + offset]->context.distance);

        // Third
        EXPECT_EQ(&third_hit_data, hits[4 + offset]->context.data);
        EXPECT_EQ(NULL, hits[4 + offset]->shared_context);
        EXPECT_EQ(third_distance + (float_t)1.0, hits[4 + offset]->context.distance);

        // Fourth
        EXPECT_EQ(&fourth_hit_data, hits[5 + offset]->context.data);
        EXPECT_EQ(NULL, hits[5 + offset]->shared_context);
        EXPECT_EQ(fourth_distance, hits[5 + offset]->context.distance);
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

    HitTesterReset(&tester, ray, (float_t)0.0);

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world);
    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world_2);
    RunPremultipliedHitTest(&tester, (float_t)0.0, nullptr);

    HitTesterReset(&tester, ray, (float_t)0.0);

    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world);
    RunPremultipliedHitTest(&tester, (float_t)0.0, model_to_world_2);
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
    PCFULL_HIT_CONTEXT *hits;
    size_t offset;
    HitTesterGetHits(tester, &hits, &offset);

    int first_hit_data = 0;
    float_t first_distance = (float_t)1.0 + min_distance;
    ISTATUS status = HitTesterTestTransformedGeometry(tester,
                                                      AllocateTwoHitAtDistance,
                                                      &first_distance,
                                                      &first_hit_data,
                                                      matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    size_t num_hits;
    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(2u + offset, num_hits);

    int second_hit_data = 0;
    float_t second_distance = (float_t)0.0 + min_distance;
    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateTwoHitAtDistance,
                                              &second_distance,
                                              &second_hit_data,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(4u + offset, num_hits);

    int third_hit_data = 0;
    float_t third_distance = (float_t)-1.0 + min_distance;
    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateTwoHitAtDistance,
                                              &third_distance,
                                              &third_hit_data,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(5u + offset, num_hits);

    int fourth_hit_data = 0;
    float_t fourth_distance = (float_t)4.0 + min_distance;
    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateHitAtDistance,
                                              &fourth_distance,
                                              &fourth_hit_data,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    EXPECT_EQ(6u + offset, num_hits);

    status = HitTesterTestTransformedGeometry(tester,
                                              AllocateNoHits,
                                              nullptr,
                                              nullptr,
                                              matrix);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    HitTesterGetHits(tester, &hits, &num_hits);
    ASSERT_EQ(6u + offset, num_hits);

    if (matrix == nullptr)
    {
        // First
        EXPECT_EQ(&first_hit_data, hits[0 + offset]->context.data);
        EXPECT_EQ(NULL, hits[0 + offset]->shared_context);
        EXPECT_EQ(first_distance, hits[0 + offset]->context.distance);
        EXPECT_EQ(&first_hit_data, hits[1 + offset]->context.data);
        EXPECT_EQ(NULL, hits[1 + offset]->shared_context);
        EXPECT_EQ(first_distance + 1.0, hits[1 + offset]->context.distance);

        // Second
        EXPECT_EQ(&second_hit_data, hits[2 + offset]->context.data);
        EXPECT_EQ(NULL, hits[2 + offset]->shared_context);
        EXPECT_EQ(second_distance, hits[2 + offset]->context.distance);
        EXPECT_EQ(&second_hit_data, hits[3 + offset]->context.data);
        EXPECT_EQ(NULL, hits[3 + offset]->shared_context);
        EXPECT_EQ(second_distance + 1.0, hits[3 + offset]->context.distance);

        // Third
        EXPECT_EQ(&third_hit_data, hits[4 + offset]->context.data);
        EXPECT_EQ(NULL, hits[4 + offset]->shared_context);
        EXPECT_EQ(third_distance + (float_t)1.0, hits[4 + offset]->context.distance);

        // Fourth
        EXPECT_EQ(&fourth_hit_data, hits[5 + offset]->context.data);
        EXPECT_EQ(NULL, hits[5 + offset]->shared_context);
        EXPECT_EQ(fourth_distance, hits[5 + offset]->context.distance);
    }
    else
    {
        // First
        EXPECT_EQ(&first_hit_data, hits[0 + offset]->context.data);
        EXPECT_FALSE(hits[0 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[0 + offset]->shared_context->model_to_world);
        EXPECT_EQ(first_distance, hits[0 + offset]->context.distance);

        EXPECT_EQ(&first_hit_data, hits[1 + offset]->context.data);
        EXPECT_FALSE(hits[1 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[1 + offset]->shared_context->model_to_world);
        EXPECT_EQ(first_distance + 1.0, hits[1 + offset]->context.distance);

        // Second
        EXPECT_EQ(&second_hit_data, hits[2 + offset]->context.data);
        EXPECT_FALSE(hits[2 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[2 + offset]->shared_context->model_to_world);
        EXPECT_EQ(second_distance, hits[2 + offset]->context.distance);

        EXPECT_EQ(&second_hit_data, hits[3 + offset]->context.data);
        EXPECT_FALSE(hits[3 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[3 + offset]->shared_context->model_to_world);
        EXPECT_EQ(second_distance + 1.0, hits[3 + offset]->context.distance);

        // Third
        EXPECT_EQ(&third_hit_data, hits[4 + offset]->context.data);
        EXPECT_FALSE(hits[4 + offset]->shared_context->premultiplied);
        EXPECT_EQ(matrix, hits[4 + offset]->shared_context->model_to_world);
        EXPECT_EQ(third_distance + 1.0, hits[4 + offset]->context.distance);

        // Fourth
        EXPECT_EQ(&fourth_hit_data, hits[5 + offset]->context.data);
        EXPECT_FALSE(hits[5 + offset]->shared_context->premultiplied);
        EXPECT_EQ(fourth_distance, hits[5 + offset]->context.distance);
        EXPECT_EQ(matrix, hits[5 + offset]->shared_context->model_to_world);
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

    HitTesterReset(&tester, ray, (float_t)0.0);

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunTransformedHitTest(&tester, 0.0, model_to_world);
    RunTransformedHitTest(&tester, 0.0, model_to_world_2);
    RunTransformedHitTest(&tester, 0.0, nullptr);

    HitTesterReset(&tester, ray, 0.0);

    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunTransformedHitTest(&tester, 0.0, model_to_world);
    RunTransformedHitTest(&tester, 0.0, model_to_world_2);
    RunTransformedHitTest(&tester, 0.0, nullptr);
    
    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}

TEST(HitTesterTest, HitTesterCheckAllHits)
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

    HitTesterReset(&tester, ray, (float_t)0.0);

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunTransformedHitTest(&tester, 0.0, model_to_world);
    RunTransformedHitTest(&tester, 0.0, model_to_world_2);
    RunTransformedHitTest(&tester, 0.0, nullptr);
    RunPremultipliedHitTest(&tester, 0.0, model_to_world);
    RunPremultipliedHitTest(&tester, 0.0, model_to_world_2);
    RunPremultipliedHitTest(&tester, 0.0, nullptr);
    RunWorldHitTest(&tester, (float_t)0.0);

    HitTesterReset(&tester, ray, 0.0);

    HitTesterGetHits(&tester, &hits, &num_hits);
    EXPECT_EQ(0u, num_hits);

    RunTransformedHitTest(&tester, 0.0, model_to_world);
    RunTransformedHitTest(&tester, 0.0, model_to_world_2);
    RunTransformedHitTest(&tester, 0.0, nullptr);
    RunPremultipliedHitTest(&tester, 0.0, model_to_world);
    RunPremultipliedHitTest(&tester, 0.0, model_to_world_2);
    RunPremultipliedHitTest(&tester, 0.0, nullptr);
    RunWorldHitTest(&tester, 0.0);
    
    HitTesterDestroy(&tester);
    MatrixRelease(model_to_world);
    MatrixRelease(model_to_world_2);
}