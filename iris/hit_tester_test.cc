/*++

Copyright (c) 2018 Brad Weinberger

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

#include "googletest/include/gtest/gtest.h"
#include "test/test_util.h"

#include <random>
#include <map>
#include <set>

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
    _Pre_null_ _Outptr_result_maybenull_ PHIT *hits
    )
{
    const NestedParams* param = static_cast<const NestedParams *>(data);
    EXPECT_EQ(param->data, data);
    EXPECT_EQ(param->ray, *ray);
    EXPECT_EQ(nullptr, *hits);
    *param->triggered = true;

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
    _Pre_null_ _Outptr_result_maybenull_ PHIT *hits
    )
{
    const ValidationParams* param = static_cast<const ValidationParams *>(data);
    EXPECT_EQ(param->data, data);
    EXPECT_EQ(param->ray, *ray);
    EXPECT_TRUE(hit_allocator);
    EXPECT_EQ(nullptr, *hits);
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

    HitTesterReset(&tester, ray);
    ISTATUS status = HitTesterTestWorldGeometry(&tester,
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

    HitTesterReset(&tester, ray);
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

TEST(HitTesterTest, HitTesterTestGeometryErrors)
{
    HIT_TESTER tester;
    ASSERT_TRUE(HitTesterInitialize(&tester));

    ISTATUS status = HitTesterTestGeometry(nullptr,
                                           CheckGeometryContext,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           true);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitTesterTestGeometry(&tester,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   true);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    HitTesterDestroy(&tester);
}

TEST(HitTesterTest, HitTesterTestGeometry)
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

    HitTesterReset(&tester, ray);
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   model_to_world,
                                   true);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   model_to_world,
                                   true);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   true);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

    triggered = false;
    params.status_to_return = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;

    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   true);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(params.triggered);

    triggered = false;
    params.status_to_return = ISTATUS_SUCCESS;
    status = HitTesterTestGeometry(&tester,
                                   CheckGeometryContext,
                                   &params,
                                   nullptr,
                                   nullptr,
                                   false);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(triggered);

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

    params.ray = RayMatrixInverseMultiply(model_to_world, ray);

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