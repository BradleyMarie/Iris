/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    shared_hit_context_allocator_test.cc

Abstract:

    Unit tests for the shared hit context allocator

--*/

extern "C" {
#include "iris/shared_hit_context_allocator.h"
}

#include <random>
#include <map>
#include <set>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(SharedHitContextAllocatorTest, TestAllocate)
{
    SHARED_HIT_CONTEXT_ALLOCATOR allocator;
    ASSERT_TRUE(SharedHitContextAllocatorInitialize(&allocator));

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PSHARED_HIT_CONTEXT context;
    ASSERT_TRUE(SharedHitContextAllocatorAllocate(&allocator,
                                                  model_to_world,
                                                  false,
                                                  &context));

    EXPECT_EQ(model_to_world, context->model_to_world);
    EXPECT_FALSE(context->premultiplied);

    ASSERT_TRUE(SharedHitContextAllocatorAllocate(&allocator,
                                                  model_to_world,
                                                  true,
                                                  &context));

    EXPECT_EQ(model_to_world, context->model_to_world);
    EXPECT_TRUE(context->premultiplied);

    SharedHitContextAllocatorDestroy(&allocator);
    MatrixRelease(model_to_world);
}

TEST(SharedHitContextAllocatorTest, TestRandom)
{
    std::minstd_rand rng(1u);
    std::uniform_int_distribution<size_t> size_generator(2,512);
    std::uniform_int_distribution<size_t> premultiplied_generator(1,2);
    std::uniform_real_distribution<> float_generator(1.0, 2.0);

    std::map<PSHARED_HIT_CONTEXT, PMATRIX> transforms;
    std::map<PSHARED_HIT_CONTEXT, bool> premultiplied;

    SHARED_HIT_CONTEXT_ALLOCATOR allocator;
    ASSERT_TRUE(SharedHitContextAllocatorInitialize(&allocator));

    for (int i = 0; i < 1000; i++)
    {
        PMATRIX model_to_world;
        ISTATUS status = MatrixAllocateTranslation(float_generator(rng),
                                                   float_generator(rng),
                                                   float_generator(rng),
                                                   &model_to_world);
        ASSERT_EQ(ISTATUS_SUCCESS, status);

        bool is_premultiplied = (premultiplied_generator(rng) == 1);
        PSHARED_HIT_CONTEXT context;
        ASSERT_TRUE(SharedHitContextAllocatorAllocate(&allocator,
                                                      model_to_world,
                                                      is_premultiplied,
                                                      &context));
        transforms[context] = model_to_world;
        premultiplied[context] = is_premultiplied;
    }

    for (const auto& entry : premultiplied)
    {
        EXPECT_EQ(entry.second, entry.first->premultiplied);
    }

    for (const auto& entry : transforms)
    {
        EXPECT_EQ(entry.second, entry.first->model_to_world);
        MatrixRelease(entry.second);
    }

    premultiplied.clear();
    transforms.clear();
    SharedHitContextAllocatorFreeAll(&allocator);

    for (int i = 0; i < 2000; i++)
    {
        PMATRIX model_to_world;
        ISTATUS status = MatrixAllocateTranslation(float_generator(rng),
                                                   float_generator(rng),
                                                   float_generator(rng),
                                                   &model_to_world);
        ASSERT_EQ(ISTATUS_SUCCESS, status);

        bool is_premultiplied = (premultiplied_generator(rng) == 1);
        PSHARED_HIT_CONTEXT context;
        ASSERT_TRUE(SharedHitContextAllocatorAllocate(&allocator,
                                                      model_to_world,
                                                      is_premultiplied,
                                                      &context));
        transforms[context] = model_to_world;
        premultiplied[context] = is_premultiplied;
    }

    for (const auto& entry : premultiplied)
    {
        EXPECT_EQ(entry.second, entry.first->premultiplied);
    }

    for (const auto& entry : transforms)
    {
        EXPECT_EQ(entry.second, entry.first->model_to_world);
        MatrixRelease(entry.second);
    }

    SharedHitContextAllocatorDestroy(&allocator);
}