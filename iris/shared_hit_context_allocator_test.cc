/*++

Copyright (c) 2018 Brad Weinberger

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
#include "test/test_util.h"

TEST(SharedHitContextAllocatorTest, TestPremultiplied)
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
    ASSERT_TRUE(SharedHitContextAllocatorAllocatePremultiplied(&allocator,
                                                               model_to_world,
                                                               &context));

    EXPECT_EQ(model_to_world, context->model_to_world);
    EXPECT_TRUE(context->premultiplied);

    SharedHitContextAllocatorDestroy(&allocator);
    MatrixRelease(model_to_world);
}

TEST(SharedHitContextAllocatorTest, TestTransformed)
{
    SHARED_HIT_CONTEXT_ALLOCATOR allocator;
    ASSERT_TRUE(SharedHitContextAllocatorInitialize(&allocator));

    PMATRIX model_to_world;
    ISTATUS status = MatrixAllocateTranslation((float_t) 1.0,
                                               (float_t) 2.0,
                                               (float_t) 3.0,
                                               &model_to_world);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0, 
                                     (float_t) 6.0);
    RAY ray = RayCreate(origin, direction);

    PSHARED_HIT_CONTEXT context;
    ASSERT_TRUE(SharedHitContextAllocatorAllocateTransformed(&allocator,
                                                             model_to_world,
                                                             ray,
                                                             &context));

    EXPECT_EQ(model_to_world, context->model_to_world);
    EXPECT_FALSE(context->premultiplied);
    EXPECT_EQ(ray, context->model_ray);

    SharedHitContextAllocatorDestroy(&allocator);
    MatrixRelease(model_to_world);
}

TEST(SharedHitContextAllocatorTest, TestRandom)
{
    std::minstd_rand rng(1u);
    std::uniform_int_distribution<size_t> size_generator(2,512);
    std::uniform_int_distribution<size_t> transformed(1,2);
    std::uniform_real_distribution<> float_generator(1.0, 2.0);

    std::map<PSHARED_HIT_CONTEXT, PMATRIX> transforms;
    std::map<PSHARED_HIT_CONTEXT, RAY> rays;
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

        if (transformed(rng) == 1)
        {
            POINT3 origin = PointCreate((float_t)float_generator(rng), 
                                        (float_t)float_generator(rng),
                                        (float_t)float_generator(rng));
            VECTOR3 direction = VectorCreate((float_t)float_generator(rng),
                                             (float_t)float_generator(rng), 
                                             (float_t)float_generator(rng));
            RAY ray = RayCreate(origin, direction);

            PSHARED_HIT_CONTEXT context;
            ASSERT_TRUE(SharedHitContextAllocatorAllocateTransformed(&allocator,
                                                                     model_to_world,
                                                                     ray,
                                                                     &context));
            transforms[context] = model_to_world;
            premultiplied[context] = false;
            rays[context] = ray;
        }
        else
        {
            PSHARED_HIT_CONTEXT context;
            ASSERT_TRUE(SharedHitContextAllocatorAllocatePremultiplied(&allocator,
                                                                       model_to_world,
                                                                       &context));
            transforms[context] = model_to_world;
            premultiplied[context] = true;
        }
    }

    for (const auto& entry : premultiplied)
    {
        EXPECT_EQ(entry.second, entry.first->premultiplied);
    }

    for (const auto& entry : rays)
    {
        EXPECT_EQ(entry.second, entry.first->model_ray);
    }

    for (const auto& entry : transforms)
    {
        EXPECT_EQ(entry.second, entry.first->model_to_world);
        MatrixRelease(entry.second);
    }

    premultiplied.clear();
    transforms.clear();
    rays.clear();
    SharedHitContextAllocatorFreeAll(&allocator);

    for (int i = 0; i < 2000; i++)
    {
        PMATRIX model_to_world;
        ISTATUS status = MatrixAllocateTranslation(float_generator(rng),
                                                    float_generator(rng),
                                                    float_generator(rng),
                                                    &model_to_world);
        ASSERT_EQ(ISTATUS_SUCCESS, status);

        if (transformed(rng) == 1)
        {
            POINT3 origin = PointCreate((float_t)float_generator(rng), 
                                        (float_t)float_generator(rng),
                                        (float_t)float_generator(rng));
            VECTOR3 direction = VectorCreate((float_t)float_generator(rng),
                                             (float_t)float_generator(rng), 
                                             (float_t)float_generator(rng));
            RAY ray = RayCreate(origin, direction);

            PSHARED_HIT_CONTEXT context;
            ASSERT_TRUE(SharedHitContextAllocatorAllocateTransformed(&allocator,
                                                                     model_to_world,
                                                                     ray,
                                                                     &context));
            transforms[context] = model_to_world;
            premultiplied[context] = false;
            rays[context] = ray;
        }
        else
        {
            PSHARED_HIT_CONTEXT context;
            ASSERT_TRUE(SharedHitContextAllocatorAllocatePremultiplied(&allocator,
                                                                       model_to_world,
                                                                       &context));
            transforms[context] = model_to_world;
            premultiplied[context] = true;
        }
    }

    for (const auto& entry : premultiplied)
    {
        EXPECT_EQ(entry.second, entry.first->premultiplied);
    }

    for (const auto& entry : rays)
    {
        EXPECT_EQ(entry.second, entry.first->model_ray);
    }

    for (const auto& entry : transforms)
    {
        EXPECT_EQ(entry.second, entry.first->model_to_world);
        MatrixRelease(entry.second);
    }

    SharedHitContextAllocatorDestroy(&allocator);
}