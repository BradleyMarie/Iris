/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    dynamic_allocator_test.cc

Abstract:

    Unit tests for dynamic_allocator.h

--*/

extern "C" {
#include "common/dynamic_allocator.h"
}

#include <cstring>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "googletest/include/gtest/gtest.h"

TEST(DynamicMemoryAllocatorTest, DynamicMemoryAllocatorInitialize)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);
    ASSERT_EQ(NULL, allocator.tail.next);
    ASSERT_EQ(NULL, allocator.head.prev);

    DynamicMemoryAllocatorDestroy(&allocator);
}

TEST(DynamicMemoryAllocatorTest, DynamicMemoryAllocatorAllocate)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);
    
    size_t size = sizeof(int);
    std::vector<int *> headers_in_order;
    std::vector<int *> data_in_order;
    for (int i = 0; i < 1000; i++)
    {
        int *header;
        int *data;
        size_t data_size = (i % 2 == 0) ? 0 : size;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   nullptr,
                                                   size,
                                                   alignof(int),
                                                   (void **)&header,
                                                   data_size,
                                                   alignof(int),
                                                   (void **)&data));

        if (data_size == 0)
        {
            EXPECT_EQ(nullptr, data);
        }

        *header = i;
        if (data)
        {
            *data = 1000 + i;
        }
        
        headers_in_order.push_back(header);
        data_in_order.push_back(data);
    }

    for (int i = 0; i < 1000; i++)
    {
        EXPECT_EQ(i, *headers_in_order[i]);
        if (data_in_order[i])
        {
            EXPECT_EQ(1000 + i, *data_in_order[i]);
        }
    }

    DynamicMemoryAllocatorDestroy(&allocator);
}

TEST(DynamicMemoryAllocatorTest, DynamicMemoryAllocatorFreeAll)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);
    
    std::set<int *> headers;
    std::set<int *> data;
    for (int i = 0; i < 1000; i++)
    {
        int *header;
        int *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   nullptr,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&header,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&datum));
        
        headers.insert(header);
        data.insert(datum);
    }

    DynamicMemoryAllocatorFreeAll(&allocator);

    for (int i = 0; i < 1000; i++)
    {
        int *header;
        int *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   nullptr,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&header,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&datum));

        EXPECT_EQ(1u, headers.erase(header));
        EXPECT_EQ(1u, data.erase(datum));
    }

    DynamicMemoryAllocatorDestroy(&allocator);
}

TEST(DynamicMemoryAllocatorTest, DynamicMemoryAllocatorFreeExcept)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);

    std::set<PDYNAMIC_ALLOCATION> handles;
    std::set<int *> headers;
    std::set<int *> data;
    for (int i = 0; i < 1000; i++)
    {
        PDYNAMIC_ALLOCATION handle;
        int *header;
        int *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   &handle,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&header,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&datum));

        handles.insert(handle);
        headers.insert(header);
        data.insert(datum);
    }

    PDYNAMIC_ALLOCATION saved_handle;
    int *saved_header;
    int *saved_datum;
    ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                               &saved_handle,
                                               sizeof(int),
                                               alignof(int),
                                               (void **)&saved_header,
                                               sizeof(int),
                                               alignof(int),
                                               (void **)&saved_datum));

    *saved_header = 0;
    *saved_datum = 1;

    DynamicMemoryAllocatorFreeAllExcept(&allocator, saved_handle);

    for (int i = 0; i < 1000; i++)
    {
        PDYNAMIC_ALLOCATION handle;
        int *header;
        int *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   &handle,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&header,
                                                   sizeof(int),
                                                   alignof(int),
                                                   (void **)&datum));

        EXPECT_NE(saved_handle, handle);
        EXPECT_NE(saved_header, header);
        EXPECT_NE(saved_datum, datum);
        EXPECT_EQ(1u, handles.erase(handle));
        EXPECT_EQ(1u, headers.erase(header));
        EXPECT_EQ(1u, data.erase(datum));
    }

    EXPECT_EQ(0, *saved_header);
    EXPECT_EQ(1, *saved_datum);

    DynamicMemoryAllocatorDestroy(&allocator);
}

std::string GenerateRandomString(size_t length, std::minstd_rand* generator)
{
    assert(length != 0);

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

TEST(DynamicMemoryAllocatorTest, RandomAllocationTest)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);

    std::minstd_rand rng(1u);
    std::uniform_int_distribution<size_t> size_generator(2,512);
    std::uniform_int_distribution<size_t> should_do_data_generator(1,2);
    std::map<char *, std::string> header_values;
    std::map<char *, std::string> data_values;

    const size_t kHeaderAlignment = 256;
    const size_t kHeaderSize = 512;

    for (int i = 0; i < 1000; i++)
    {
        size_t header_size = kHeaderSize;

        size_t datum_size;
        if (should_do_data_generator(rng) != 1)
        {
            datum_size = size_generator(rng);
        }
        else
        {
            datum_size = 0;
        }

        char *header;
        char *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   nullptr,
                                                   header_size,
                                                   kHeaderAlignment,
                                                   (void **)&header,
                                                   datum_size,
                                                   GetAlignment(datum_size),
                                                   (void **)&datum));

        EXPECT_EQ(0u, (uintptr_t)header % kHeaderAlignment);
        EXPECT_EQ(0u, (uintptr_t)datum % GetAlignment(datum_size));

        std::string header_value = GenerateRandomString(header_size, &rng);

        strncpy(header, header_value.c_str(), header_size);
        header_values[header] = header_value;

        if (datum_size != 0)
        {
            std::string datum_value = GenerateRandomString(datum_size, &rng);
            strncpy(datum, datum_value.c_str(), datum_size);
            data_values[datum] = datum_value;
        }
    }

    EXPECT_EQ(1000u, header_values.size());

    for (const auto& entry : header_values)
    {
        EXPECT_STREQ(entry.second.c_str(), entry.first);
    }

    header_values.clear();

    for (const auto& entry : data_values)
    {
        EXPECT_STREQ(entry.second.c_str(), entry.first);
    }

    data_values.clear();

    DynamicMemoryAllocatorFreeAll(&allocator);

    for (int i = 0; i < 2000; i++)
    {
        size_t header_size = kHeaderSize;

        size_t datum_size;
        if (should_do_data_generator(rng) != 1)
        {
            datum_size = size_generator(rng);
        }
        else
        {
            datum_size = 0;
        }

        char *header;
        char *datum;
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   nullptr,
                                                   header_size,
                                                   kHeaderAlignment,
                                                   (void **)&header,
                                                   datum_size,
                                                   GetAlignment(datum_size),
                                                   (void **)&datum));

        EXPECT_EQ(0u, (uintptr_t)header % kHeaderAlignment);
        EXPECT_EQ(0u, (uintptr_t)datum % GetAlignment(datum_size));

        std::string header_value = GenerateRandomString(header_size, &rng);

        strncpy(header, header_value.c_str(), header_size);
        header_values[header] = header_value;

        if (datum_size)
        {
            std::string datum_value = GenerateRandomString(datum_size, &rng);
            strncpy(datum, datum_value.c_str(), datum_size);
            data_values[datum] = datum_value;
        }
    }

    EXPECT_EQ(2000u, header_values.size());

    for (const auto& entry : header_values)
    {
        EXPECT_STREQ(entry.second.c_str(), entry.first);
    }

    header_values.clear();

    for (const auto& entry : data_values)
    {
        EXPECT_STREQ(entry.second.c_str(), entry.first);
    }

    data_values.clear();

    DynamicMemoryAllocatorDestroy(&allocator);
}