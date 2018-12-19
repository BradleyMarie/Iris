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

TEST(RandomTest, RandomAllocateErrors)
{
    RANDOM_VTABLE vtable;
    PRANDOM rng;

    ISTATUS status = RandomAllocate(nullptr, 
                                    &vtable,
                                    sizeof(vtable),
                                    alignof(vtable),
                                    &rng);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = RandomAllocate(&vtable, 
                            nullptr,
                            sizeof(vtable),
                            alignof(vtable),
                            &rng);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    status = RandomAllocate(&vtable, 
                            &vtable,
                            sizeof(vtable),
                            0,
                            &rng);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = RandomAllocate(&vtable, 
                            &vtable,
                            sizeof(vtable),
                            alignof(vtable) + 5,
                            &rng);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = RandomAllocate(&vtable, 
                            &vtable,
                            sizeof(vtable),
                            1024,
                            &rng);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);

    status = RandomAllocate(&vtable, 
                            &vtable,
                            sizeof(vtable),
                            alignof(vtable),
                            nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);
}

TEST(RandomTest, RandomGenerateFloatErrors)
{
    RANDOM_VTABLE vtable = { nullptr, nullptr, nullptr };
    PRANDOM rng;

    ISTATUS status = RandomAllocate(&vtable, 
                                    nullptr,
                                    0,
                                    0,
                                    &rng);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t result;
    status = RandomGenerateFloat(nullptr,
                                 (float_t)0.0,
                                 (float_t)1.0,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)-INFINITY,
                                 (float_t)1.0,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    
    status = RandomGenerateFloat(rng,
                                 std::numeric_limits<float_t>::quiet_NaN(),
                                 (float_t)1.0,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)INFINITY,
                                 (float_t)INFINITY,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)0.0,
                                 (float_t)-INFINITY,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)1.0,
                                 std::numeric_limits<float_t>::quiet_NaN(),
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)1.0,
                                 (float_t)INFINITY,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)1.0,
                                 (float_t)0.0,
                                 &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);
    
    status = RandomGenerateFloat(rng,
                                 (float_t)0.0,
                                 (float_t)1.0,
                                 nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    RandomFree(rng);
}

TEST(RandomTest, RandomGenerateIndexErrors)
{
    RANDOM_VTABLE vtable = { nullptr, nullptr, nullptr };
    PRANDOM rng;

    ISTATUS status = RandomAllocate(&vtable, 
                                    nullptr,
                                    0,
                                    0,
                                    &rng);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    size_t result;
    status = RandomGenerateIndex(nullptr, 1, &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = RandomGenerateIndex(rng, 0, &result);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = RandomGenerateIndex(rng, 1, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    RandomFree(rng);
}

struct FloatContext {
    ISTATUS return_status;
    float_t min;
    float_t max;
    float_t return_value;
    bool *generate_encountered;
    bool *free_encountered;
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
    EXPECT_FALSE(*float_context->generate_encountered);
    *float_context->generate_encountered = true;
    return float_context->return_status;
}

void
TestGenerateFloatFreeCallback(
    _In_opt_ _Post_invalid_ void *context
    )
{
    FloatContext *float_context = static_cast<FloatContext*>(context);
    EXPECT_FALSE(*float_context->free_encountered);
    *float_context->free_encountered = true;
}

void
TestGenerateFloat(
    _In_ float_t minimum,
    _In_ float_t maximum,
    _In_ float_t return_value,
    _In_ ISTATUS return_status
    )
{
    bool generate_encountered = false;
    bool free_encountered = false;

    RANDOM_VTABLE vtable = { TestGenerateFloatCallback,
                             nullptr,
                             TestGenerateFloatFreeCallback };
    FloatContext context = { return_status,
                             minimum,
                             maximum,
                             return_value,
                             &generate_encountered,
                             &free_encountered };
    PRANDOM rng;

    ISTATUS status = RandomAllocate(&vtable,
                                    &context,
                                    sizeof(context),
                                    alignof(context),
                                    &rng);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_FALSE(generate_encountered);
    float_t result;
    status = RandomGenerateFloat(rng, minimum, maximum, &result);
    EXPECT_TRUE(generate_encountered);
    EXPECT_EQ(return_status, status);

    if (status == ISTATUS_SUCCESS)
    {
        EXPECT_EQ(return_value, result);
    }

    EXPECT_FALSE(free_encountered);
    RandomFree(rng);
    EXPECT_TRUE(free_encountered);
}

TEST(RandomTest, RandomGenerateFloat)
{
    TestGenerateFloat(0.0, 1.0, 0.5, ISTATUS_SUCCESS);
    TestGenerateFloat(1.0, 2.0, 1.75, ISTATUS_SUCCESS);
    TestGenerateFloat(0.0, 1.0, 0.75, ISTATUS_INVALID_ARGUMENT_31);
}

struct IndexContext {
    ISTATUS return_status;
    size_t upper_bound;
    size_t return_value;
    bool *generate_encountered;
    bool *free_encountered;
};

ISTATUS
TestGenerateIndexCallback(
    _In_ void *context,
    _In_ size_t upper_bound,
    _Out_range_(0, upper_bound - 1) size_t *result
    )
{
    IndexContext *index_context = static_cast<IndexContext*>(context);
    EXPECT_EQ(index_context->upper_bound, upper_bound);
    EXPECT_TRUE(result != NULL);
    *result = index_context->return_value;
    EXPECT_FALSE(*index_context->generate_encountered);
    *index_context->generate_encountered = true;
    return index_context->return_status;
}

void
TestGenerateIndexFreeCallback(
    _In_opt_ _Post_invalid_ void *context
    )
{
    IndexContext *index_context = static_cast<IndexContext*>(context);
    EXPECT_FALSE(*index_context->free_encountered);
    *index_context->free_encountered = true;
}

void
TestGenerateIndex(
    _In_ size_t upper_bound,
    _In_ size_t return_value,
    _In_ ISTATUS return_status
    )
{
    bool generate_encountered = false;
    bool free_encountered = false;

    RANDOM_VTABLE vtable = { nullptr,
                             TestGenerateIndexCallback,
                             TestGenerateIndexFreeCallback };
    IndexContext context = { return_status,
                             upper_bound,
                             return_value,
                             &generate_encountered,
                             &free_encountered };
    PRANDOM rng;

    ISTATUS status = RandomAllocate(&vtable,
                                    &context,
                                    sizeof(context),
                                    alignof(context),
                                    &rng);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    EXPECT_FALSE(generate_encountered);
    size_t result;
    status = RandomGenerateIndex(rng, upper_bound, &result);
    EXPECT_TRUE(generate_encountered);
    EXPECT_EQ(return_status, status);

    if (status == ISTATUS_SUCCESS)
    {
        EXPECT_EQ(return_value, result);
    }

    EXPECT_FALSE(free_encountered);
    RandomFree(rng);
    EXPECT_TRUE(free_encountered);
}

TEST(RandomTest, RandomGenerateIndex)
{
    TestGenerateIndex(2, 1, ISTATUS_SUCCESS);
    TestGenerateIndex(2, 1, ISTATUS_INVALID_ARGUMENT_31);
}