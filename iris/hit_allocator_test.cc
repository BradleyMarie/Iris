/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    hit_allocator_test.cc

Abstract:

    Unit tests for the hit allocator

--*/

extern "C" {
#include "iris/full_hit_context.h"
#include "iris/hit_allocator.h"
#include "iris/hit_allocator_internal.h"
}

#include <cstring>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "test_util/equality.h"

TEST(HitAllocatorTest, HitAllocatorSetRayGetRay)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    POINT3 origin = PointCreate((float_t) 1.0, (float_t) 2.0, (float_t) 3.0);
    VECTOR3 direction = VectorCreate((float_t) 4.0,
                                     (float_t) 5.0,
                                     (float_t) 6.0);
    RAY model_ray = RayCreate(origin, direction);
    HitAllocatorSetRay(&allocator, &model_ray);
    EXPECT_EQ(&model_ray, HitAllocatorGetRay(&allocator));

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorSetDataGetData)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    POINT3 data;
    HitAllocatorSetData(&allocator, &data);
    EXPECT_EQ(&data, HitAllocatorGetData(&allocator));

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocateErrors)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    void *additional_data = nullptr;
    size_t additional_data_size = 0;
    size_t additional_data_alignment = 0;
    PHIT hit;
    ISTATUS status = HitAllocatorAllocate(nullptr,
                                          next,
                                          distance,
                                          front_face,
                                          back_face,
                                          additional_data,
                                          additional_data_size,
                                          additional_data_alignment,
                                          &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  INFINITY,
                                  front_face,
                                  back_face,
                                  additional_data,
                                  additional_data_size,
                                  additional_data_alignment,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  -INFINITY,
                                  front_face,
                                  back_face,
                                  additional_data,
                                  additional_data_size,
                                  additional_data_alignment,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  std::numeric_limits<float_t>::quiet_NaN(),
                                  front_face,
                                  back_face,
                                  additional_data,
                                  additional_data_size,
                                  additional_data_alignment,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  distance,
                                  front_face,
                                  back_face,
                                  additional_data,
                                  additional_data_size,
                                  additional_data_alignment,
                                  nullptr);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_08, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  distance,
                                  front_face,
                                  back_face,
                                  additional_data,
                                  sizeof(int),
                                  additional_data_alignment,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    int data;
    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  distance,
                                  front_face,
                                  back_face,
                                  &data,
                                  sizeof(int),
                                  0,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  distance,
                                  front_face,
                                  back_face,
                                  &data,
                                  sizeof(int),
                                  5,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = HitAllocatorAllocate(&allocator,
                                  next,
                                  distance,
                                  front_face,
                                  back_face,
                                  &data,
                                  sizeof(int),
                                  8,
                                  &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocate)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    void *additional_data = nullptr;
    size_t additional_data_size = 0;
    size_t additional_data_alignment = 0;
    PHIT hit;
    ISTATUS status = HitAllocatorAllocate(&allocator,
                                          next,
                                          distance,
                                          front_face,
                                          back_face,
                                          additional_data,
                                          additional_data_size,
                                          additional_data_alignment,
                                          &hit);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(distance, hit->distance);
    ASSERT_EQ(nullptr, hit->next);

    PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hit;

    ASSERT_EQ(nullptr, full_hit->context.data);
    ASSERT_EQ(distance, full_hit->context.distance);
    ASSERT_EQ(front_face, full_hit->context.front_face);
    ASSERT_EQ(back_face, full_hit->context.back_face);
    ASSERT_EQ(additional_data, full_hit->context.additional_data);
    ASSERT_EQ(additional_data_size, full_hit->context.additional_data_size);
    ASSERT_FALSE(full_hit->model_hit_point_valid);

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocateWithAdditionalData)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    int data = 4;
    void *additional_data = &data;
    size_t additional_data_size = sizeof(int);
    size_t additional_data_alignment = alignof(int);
    PHIT hit;
    ISTATUS status = HitAllocatorAllocate(&allocator,
                                          next,
                                          distance,
                                          front_face,
                                          back_face,
                                          additional_data,
                                          additional_data_size,
                                          additional_data_alignment,
                                          &hit);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(distance, hit->distance);
    ASSERT_EQ(nullptr, hit->next);

    PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hit;

    ASSERT_EQ(nullptr, full_hit->context.data);
    ASSERT_EQ(distance, full_hit->context.distance);
    ASSERT_EQ(front_face, full_hit->context.front_face);
    ASSERT_EQ(back_face, full_hit->context.back_face);
    ASSERT_EQ(data, *(int *)full_hit->context.additional_data);
    ASSERT_EQ(additional_data_size, full_hit->context.additional_data_size);
    ASSERT_FALSE(full_hit->model_hit_point_valid);

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocateWithHitPointErrors)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    void *additional_data = nullptr;
    size_t additional_data_size = 0;
    size_t additional_data_alignment = 0;
    POINT3 hit_point = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    PHIT hit;
    ISTATUS status = HitAllocatorAllocateWithHitPoint(nullptr,
                                                      next,
                                                      distance,
                                                      front_face,
                                                      back_face,
                                                      additional_data,
                                                      additional_data_size,
                                                      additional_data_alignment,
                                                      hit_point,
                                                      &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              INFINITY,
                                              front_face,
                                              back_face,
                                              additional_data,
                                              additional_data_size,
                                              additional_data_alignment,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              -INFINITY,
                                              front_face,
                                              back_face,
                                              additional_data,
                                              additional_data_size,
                                              additional_data_alignment,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              std::numeric_limits<float_t>::quiet_NaN(),
                                              front_face,
                                              back_face,
                                              additional_data,
                                              additional_data_size,
                                              additional_data_alignment,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    hit_point.x = std::numeric_limits<float_t>::quiet_NaN();
    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              additional_data,
                                              additional_data_size,
                                              additional_data_alignment,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_08, status);
    hit_point.x = 0;

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              additional_data,
                                              additional_data_size,
                                              additional_data_alignment,
                                              hit_point,
                                              nullptr);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_09, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              additional_data,
                                              sizeof(int),
                                              additional_data_alignment,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    int data;
    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              &data,
                                              sizeof(int),
                                              0,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              &data,
                                              sizeof(int),
                                              5,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    status = HitAllocatorAllocateWithHitPoint(&allocator,
                                              next,
                                              distance,
                                              front_face,
                                              back_face,
                                              &data,
                                              sizeof(int),
                                              8,
                                              hit_point,
                                              &hit);
    ASSERT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocateWithHitPoint)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    void *additional_data = nullptr;
    size_t additional_data_size = 0;
    size_t additional_data_alignment = 0;
    POINT3 hit_point = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    PHIT hit;
    ISTATUS status = HitAllocatorAllocateWithHitPoint(&allocator,
                                                      next,
                                                      distance,
                                                      front_face,
                                                      back_face,
                                                      additional_data,
                                                      additional_data_size,
                                                      additional_data_alignment,
                                                      hit_point,
                                                      &hit);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(distance, hit->distance);
    ASSERT_EQ(nullptr, hit->next);

    PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hit;

    ASSERT_EQ(nullptr, full_hit->context.data);
    ASSERT_EQ(distance, full_hit->context.distance);
    ASSERT_EQ(front_face, full_hit->context.front_face);
    ASSERT_EQ(back_face, full_hit->context.back_face);
    ASSERT_EQ(additional_data, full_hit->context.additional_data);
    ASSERT_EQ(additional_data_size, full_hit->context.additional_data_size);
    ASSERT_TRUE(full_hit->model_hit_point_valid);
    ASSERT_EQ(hit_point, full_hit->model_hit_point);

    HitAllocatorDestroy(&allocator);
}

TEST(HitAllocatorTest, HitAllocatorAllocateWithHitPointAndAdditionalData)
{
    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT next = nullptr;
    float_t distance = (float_t)1.0;
    uint32_t front_face = 0;
    uint32_t back_face = 1;
    int data = 4;
    void *additional_data = &data;
    size_t additional_data_size = sizeof(int);
    size_t additional_data_alignment = alignof(int);
    POINT3 hit_point = PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
    PHIT hit;
    ISTATUS status = HitAllocatorAllocateWithHitPoint(&allocator,
                                                      next,
                                                      distance,
                                                      front_face,
                                                      back_face,
                                                      additional_data,
                                                      additional_data_size,
                                                      additional_data_alignment,
                                                      hit_point,
                                                      &hit);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(distance, hit->distance);
    ASSERT_EQ(nullptr, hit->next);

    PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hit;

    ASSERT_EQ(nullptr, full_hit->context.data);
    ASSERT_EQ(distance, full_hit->context.distance);
    ASSERT_EQ(front_face, full_hit->context.front_face);
    ASSERT_EQ(back_face, full_hit->context.back_face);
    ASSERT_EQ(data, *(int *)full_hit->context.additional_data);
    ASSERT_EQ(additional_data_size, full_hit->context.additional_data_size);
    ASSERT_TRUE(full_hit->model_hit_point_valid);
    ASSERT_EQ(hit_point, full_hit->model_hit_point);

    HitAllocatorDestroy(&allocator);
}

std::string GenerateRandomString(size_t length, std::minstd_rand* generator)
{
    if (length == 0)
    {
        return std::string();
    }

    std::uniform_int_distribution<char> character_generator(97,122);

    std::string output;
    for (size_t i = 0; i < length - 1; i++)
    {
        output += character_generator(*generator);
    }

    return output;
}

size_t GetAlignment(size_t size)
{
    if (size % 512 == 0)
    {
        return 512;
    }

    if (size % 256 == 0)
    {
        return 256;
    }

    if (size % 128 == 0)
    {
        return 128;
    }

    if (size % 64 == 0)
    {
        return 64;
    }

    if (size % 32 == 0)
    {
        return 32;
    }

    if (size % 16 == 0)
    {
        return 16;
    }

    if (size % 8 == 0)
    {
        return 8;
    }

    if (size % 4 == 0)
    {
        return 4;
    }

    if (size % 2 == 0)
    {
        return 2;
    }

    return 1;
}

TEST(HitAllocatorTest, HitAllocatorTestRepeatedAllocation)
{
    std::minstd_rand rng(1u);
    std::uniform_int_distribution<size_t> size_generator(2,512);
    std::uniform_int_distribution<size_t> should_do_action(1,16);
    std::uniform_int_distribution<uint32_t> face_generator(1,1000);
    std::uniform_real_distribution<> float_generator(-2.0, 2.0);

    std::map<PHIT, PHIT> nexts;
    std::map<PHIT, float_t> distances;
    std::map<PHIT, uint32_t> front_faces;
    std::map<PHIT, uint32_t> back_faces;
    std::map<PHIT, POINT3> hit_points;
    std::set<PHIT> no_hit_points;
    std::map<PHIT, std::string> data;
    std::map<PHIT, size_t> data_sizes;
    std::vector<PHIT> hits_in_order;

    HIT_ALLOCATOR allocator;
    HitAllocatorInitialize(&allocator);

    PHIT last = nullptr;
    for (uintptr_t i = 0; i < 1000; i++)
    {
        size_t datum_size = 0;
        if (should_do_action(rng) != 1)
        {
            datum_size = size_generator(rng);
        }

        PHIT next = nullptr;
        if (should_do_action(rng) != 1)
        {
            next = last;
        }

        std::string random_string = GenerateRandomString(datum_size, &rng);

        float_t distance = (float_t)float_generator(rng);
        uint32_t front_face = face_generator(rng);
        uint32_t back_face = face_generator(rng);
        size_t additional_data_size = datum_size;
        size_t additional_data_alignment = GetAlignment(datum_size);

        const void *additional_data = nullptr;
        if (datum_size != 0)
        {
            additional_data = random_string.c_str();
        }

        HitAllocatorSetData(&allocator, (const void *)i);

        PHIT hit;
        if (should_do_action(rng) < 8) {
            POINT3 hit_point = PointCreate((float_t)float_generator(rng), 
                                           (float_t)float_generator(rng), 
                                           (float_t)float_generator(rng));
            ISTATUS status = HitAllocatorAllocateWithHitPoint(&allocator,
                                                              next,
                                                              distance,
                                                              front_face,
                                                              back_face,
                                                              additional_data,
                                                              additional_data_size,
                                                              additional_data_alignment,
                                                              hit_point,
                                                              &hit);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            hit_points[hit] = hit_point;
        }
        else
        {
            ISTATUS status = HitAllocatorAllocate(&allocator,
                                                  next,
                                                  distance,
                                                  front_face,
                                                  back_face,
                                                  additional_data,
                                                  additional_data_size,
                                                  additional_data_alignment,
                                                  &hit);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            no_hit_points.insert(hit);
        }

        hits_in_order.push_back(hit);
        nexts[hit] = next;
        distances[hit] = distance;
        front_faces[hit] = front_face;
        back_faces[hit] = back_face;
        data_sizes[hit] = additional_data_size;

        if (additional_data_size != 0)
        {
            data[hit] = random_string;
        }

        last = hit;
    }

    EXPECT_EQ(1000u, hits_in_order.size());

    for (const auto& entry : nexts)
    {
        EXPECT_EQ(entry.second, entry.first->next);
    }

    for (const auto& entry : distances)
    {
        EXPECT_EQ(entry.second, entry.first->distance);
    }

    for (uintptr_t i = 0; i < hits_in_order.size(); i++)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hits_in_order[i];
        EXPECT_EQ(i, (uintptr_t)full_hit->context.data);
    }

    for (const auto& entry : front_faces)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.front_face);
    }

    for (const auto& entry : back_faces)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.back_face);
    }

    for (const auto& entry : data_sizes)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.additional_data_size);

        if (entry.second == 0)
        {
            EXPECT_EQ(nullptr, full_hit->context.additional_data);
        }
    }

    for (const auto& entry : data)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_STREQ(entry.second.c_str(), (const char *)full_hit->context.additional_data);
        EXPECT_EQ(0u, (uintptr_t)full_hit->context.additional_data % GetAlignment(full_hit->context.additional_data_size));
    }

    for (const auto& entry : no_hit_points)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry;
        EXPECT_FALSE(full_hit->model_hit_point_valid);
    }

    for (const auto& entry : hit_points)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_TRUE(full_hit->model_hit_point_valid);
        EXPECT_EQ(entry.second, full_hit->model_hit_point);
    }

    nexts.clear();
    distances.clear();
    front_faces.clear();
    back_faces.clear();
    hit_points.clear();
    no_hit_points.clear();
    data.clear();
    data_sizes.clear();
    hits_in_order.clear();
    HitAllocatorFreeAll(&allocator);

    last = nullptr;
    for (uintptr_t i = 0; i < 2000; i++)
    {
        size_t datum_size = 0;
        if (should_do_action(rng) != 1)
        {
            datum_size = size_generator(rng);
        }

        PHIT next = nullptr;
        if (should_do_action(rng) != 1)
        {
            next = last;
        }

        std::string random_string = GenerateRandomString(datum_size, &rng);

        float_t distance = (float_t)float_generator(rng);
        uint32_t front_face = face_generator(rng);
        uint32_t back_face = face_generator(rng);
        size_t additional_data_size = datum_size;
        size_t additional_data_alignment = GetAlignment(datum_size);

        const void *additional_data = nullptr;
        if (datum_size != 0)
        {
            additional_data = random_string.c_str();
        }

        HitAllocatorSetData(&allocator, (const void *)i);

        PHIT hit;
        if (should_do_action(rng) < 8) {
            POINT3 hit_point = PointCreate((float_t)float_generator(rng), 
                                           (float_t)float_generator(rng), 
                                           (float_t)float_generator(rng));
            ISTATUS status = HitAllocatorAllocateWithHitPoint(&allocator,
                                                              next,
                                                              distance,
                                                              front_face,
                                                              back_face,
                                                              additional_data,
                                                              additional_data_size,
                                                              additional_data_alignment,
                                                              hit_point,
                                                              &hit);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            hit_points[hit] = hit_point;
        }
        else
        {
            ISTATUS status = HitAllocatorAllocate(&allocator,
                                                  next,
                                                  distance,
                                                  front_face,
                                                  back_face,
                                                  additional_data,
                                                  additional_data_size,
                                                  additional_data_alignment,
                                                  &hit);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            no_hit_points.insert(hit);
        }

        hits_in_order.push_back(hit);
        nexts[hit] = next;
        distances[hit] = distance;
        front_faces[hit] = front_face;
        back_faces[hit] = back_face;
        data_sizes[hit] = additional_data_size;

        if (additional_data_size != 0)
        {
            data[hit] = random_string;
        }

        last = hit;
    }

    EXPECT_EQ(2000u, hits_in_order.size());

    for (const auto& entry : nexts)
    {
        EXPECT_EQ(entry.second, entry.first->next);
    }

    for (const auto& entry : distances)
    {
        EXPECT_EQ(entry.second, entry.first->distance);
    }

    for (uintptr_t i = 0; i < hits_in_order.size(); i++)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)hits_in_order[i];
        EXPECT_EQ(i, (uintptr_t)full_hit->context.data);
    }

    for (const auto& entry : front_faces)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.front_face);
    }

    for (const auto& entry : back_faces)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.back_face);
    }

    for (const auto& entry : data_sizes)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_EQ(entry.second, full_hit->context.additional_data_size);

        if (entry.second == 0)
        {
            EXPECT_EQ(nullptr, full_hit->context.additional_data);
        }
    }

    for (const auto& entry : data)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_STREQ(entry.second.c_str(), (const char *)full_hit->context.additional_data);
        EXPECT_EQ(0u, (uintptr_t)full_hit->context.additional_data % GetAlignment(full_hit->context.additional_data_size));
    }

    for (const auto& entry : no_hit_points)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry;
        EXPECT_FALSE(full_hit->model_hit_point_valid);
    }

    for (const auto& entry : hit_points)
    {
        PCFULL_HIT_CONTEXT full_hit = (PCFULL_HIT_CONTEXT)(const void *)entry.first;
        EXPECT_TRUE(full_hit->model_hit_point_valid);
        EXPECT_EQ(entry.second, full_hit->model_hit_point);
    }

    HitAllocatorDestroy(&allocator);
}