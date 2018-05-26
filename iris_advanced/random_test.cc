/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random_test.cc

Abstract:

    Unit tests for random.c

--*/

extern "C" {
#include "iris_advanced/random.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"

struct FloatContext {
    ISTATUS return_status;
    float_t min;
    float_t max;
    float_t return_value;
    bool encountered;
};

ISTATUS
TestGenerateFloatCallback(
    _In_ void *context,
    _In_ float_t minimum,
    _In_ float_t maximum,
    _Out_range_(minimum, maximum) float_t *result
    )
{
    FloatContext *float_context = static_cast<FloatContext*>(context);
    EXPECT_EQ(float_context->min, minimum);
    EXPECT_EQ(float_context->max, maximum);
    EXPECT_TRUE(result != NULL);
    *result = float_context->return_value;
    EXPECT_FALSE(float_context->encountered);
    float_context->encountered = true;
    return float_context->return_status;
}

struct IndexContext {
    ISTATUS return_status;
    float_t min;
    float_t max;
    float_t return_value;
    bool encountered;
};

ISTATUS
TestGenerateIndexCallback(
    _In_ void *context,
    _In_ size_t minimum,
    _In_ size_t maximum,
    _Out_range_(minimum, maximum) size_t *result
    )
{
    IndexContext *index_context = static_cast<IndexContext*>(context);
    EXPECT_EQ(index_context->min, minimum);
    EXPECT_EQ(index_context->max, maximum);
    EXPECT_TRUE(result != NULL);
    *result = index_context->return_value;
    EXPECT_FALSE(index_context->encountered);
    index_context->encountered = true;
    return index_context->return_status;
}

ISTATUS
TestGenerateIndexLifetimeCallback(
    _Inout_opt_ void *context,
    _In_ PRANDOM rng
    )
{
    IndexContext *index_context = static_cast<IndexContext*>(context);

    size_t value;
    ISTATUS status = RandomGenerateIndex(rng,
                                         index_context->min,
                                         index_context->max,
                                         &value);
    EXPECT_EQ(index_context->return_status, status);
    EXPECT_EQ(index_context->return_value, value);
    EXPECT_TRUE(index_context->encountered);

    return ISTATUS_SUCCESS;
}

ISTATUS
TestGenerateFloatLifetimeCallback(
    _Inout_opt_ void *context,
    _In_ PRANDOM rng
    )
{
    FloatContext *float_context = static_cast<FloatContext*>(context);

    float_t value;
    ISTATUS status = RandomGenerateFloat(rng,
                                         float_context->min,
                                         float_context->max,
                                         &value);
    EXPECT_EQ(float_context->return_status, status);
    EXPECT_EQ(float_context->return_value, value);
    EXPECT_TRUE(float_context->encountered);

    return ISTATUS_SUCCESS;
}

TEST(RandomTest, RandomCreateErrors)
{
    ISTATUS status = RandomCreate(NULL,
                                  TestGenerateIndexCallback,
                                  NULL,
                                  TestGenerateIndexLifetimeCallback,
                                  NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = RandomCreate(TestGenerateFloatCallback,
                          NULL,
                          NULL,
                          TestGenerateIndexLifetimeCallback,
                          NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    
    status = RandomCreate(TestGenerateFloatCallback,
                          TestGenerateIndexCallback,
                          NULL,
                          NULL,
                          NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
}

struct TestContext {
    ISTATUS return_status;
    bool encountered;
};

ISTATUS
TestRandomCreateLifetimeCallback(
    _Inout_opt_ void *context,
    _In_ PRANDOM rng
    )
{
    TestContext *test_context = static_cast<TestContext*>(context);

    size_t index;
    ISTATUS status = RandomGenerateIndex(NULL, 0, 100, &index);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);
    
    status = RandomGenerateIndex(rng, 0, 100, NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RandomGenerateIndex(rng, 100, 0, &index);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    float_t float_value;
    status = RandomGenerateFloat(NULL, 0.0, 100.0, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = RandomGenerateFloat(rng, -INFINITY, 100.0, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = RandomGenerateFloat(rng, INFINITY, INFINITY, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = RandomGenerateFloat(rng, 
                                 std::numeric_limits<float_t>::quiet_NaN(), 
                                 100.0, 
                                 &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = RandomGenerateFloat(rng, 0.0, -INFINITY, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RandomGenerateFloat(rng, 0.0, INFINITY, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RandomGenerateFloat(rng, 
                                 0.0,
                                 std::numeric_limits<float_t>::quiet_NaN(), 
                                 &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RandomGenerateFloat(rng, 0.0, 100.0, NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RandomGenerateFloat(rng, 100.0, 0.0, &float_value);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    EXPECT_FALSE(test_context->encountered);
    test_context->encountered = true;
    return test_context->return_status;
}

TEST(RandomTest, RandomCreateAndGenerateErrors)
{
    TestContext context;
    context.return_status = ISTATUS_SUCCESS;
    context.encountered = false;
    ISTATUS status = RandomCreate(TestGenerateFloatCallback,
                                  TestGenerateIndexCallback,
                                  NULL,
                                  TestRandomCreateLifetimeCallback,
                                  &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(context.encountered);

    context.return_status = ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
    context.encountered = false;
    status = RandomCreate(TestGenerateFloatCallback,
                          TestGenerateIndexCallback,
                          NULL,
                          TestRandomCreateLifetimeCallback,
                          &context);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);
    EXPECT_TRUE(context.encountered);
}

void
TestGenerateFloat(
    float_t minimum,
    float_t maximum,
    float_t return_value,
    ISTATUS return_status
    )
{
    FloatContext context;
    context.min = minimum;
    context.max = maximum; 
    context.return_value = return_value;
    context.return_status = return_status;
    context.encountered = false;

    ISTATUS status = RandomCreate(TestGenerateFloatCallback,
                                  TestGenerateIndexCallback,
                                  &context,
                                  TestGenerateFloatLifetimeCallback,
                                  &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
}

TEST(RandomTest, RandomGenerateFloat)
{
    TestGenerateFloat(0.0, 1.0, 0.5, ISTATUS_SUCCESS);
    TestGenerateFloat(0.0, 1.0, 0.75, ISTATUS_INVALID_ARGUMENT_31);
}

void
TestGenerateIndex(
    size_t minimum,
    size_t maximum,
    size_t return_value,
    ISTATUS return_status
    )
{
    IndexContext context;
    context.min = minimum;
    context.max = maximum; 
    context.return_value = return_value;
    context.return_status = return_status;
    context.encountered = false;

    ISTATUS status = RandomCreate(TestGenerateFloatCallback,
                                  TestGenerateIndexCallback,
                                  &context,
                                  TestGenerateIndexLifetimeCallback,
                                  &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
}

TEST(RandomTest, RandomGenerateIndex)
{
    TestGenerateIndex(0, 100, 50, ISTATUS_SUCCESS);
    TestGenerateIndex(0, 100, 75, ISTATUS_INVALID_ARGUMENT_31);
}