/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    dynamic_allocator_test.cc

Abstract:

    Unit tests for dynamic_allocator.h

--*/

extern "C" {
#include "common/dynamic_allocator.h"
}

#include "googletest/include/gtest/gtest.h"

#include <cstring>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

TEST(DynamicMemoryAllocatorTest, DynamicMemoryAllocatorInitialize)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);
    ASSERT_EQ(NULL, allocator.last_allocation);
    ASSERT_EQ(NULL, allocator.next_allocation);

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
        ASSERT_TRUE(DynamicMemoryAllocatorAllocate(&allocator,
                                                   size,
                                                   alignof(int),
                                                   (void **)&header,
                                                   (i % 2 == 0) ? 0 : size,
                                                   alignof(int),
                                                   (void **)&data));
        
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

TEST(DynamicMemoryAllocatorTest, RandomAllocationTest)
{
    DYNAMIC_MEMORY_ALLOCATOR allocator;
    DynamicMemoryAllocatorInitialize(&allocator);

    std::minstd_rand rng(1u);
    std::uniform_int_distribution<size_t> size_generator(2,512);
    std::uniform_int_distribution<size_t> should_do_data_generator(1,16);
    std::map<char *, std::string> header_values;
    std::map<char *, std::string> data_values;

    for (int i = 0; i < 1000; i++)
    {
        size_t header_size = size_generator(rng);

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
                                                   header_size,
                                                   GetAlignment(header_size),
                                                   (void **)&header,
                                                   datum_size,
                                                   GetAlignment(datum_size),
                                                   (void **)&datum));

        EXPECT_EQ(0u, (uintptr_t)header % GetAlignment(header_size));
        EXPECT_EQ(0u, (uintptr_t)datum % GetAlignment(datum_size));

        std::string header_value = GenerateRandomString(header_size, &rng);
        std::string datum_value = GenerateRandomString(datum_size, &rng);

        strncpy(header, header_value.c_str(), header_size);
        header_values[header] = header_value;

        if (datum_size)
        {
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
        size_t header_size = size_generator(rng);

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
                                                   header_size,
                                                   GetAlignment(header_size),
                                                   (void **)&header,
                                                   datum_size,
                                                   GetAlignment(datum_size),
                                                   (void **)&datum));

        EXPECT_EQ(0u, (uintptr_t)header % GetAlignment(header_size));
        EXPECT_EQ(0u, (uintptr_t)datum % GetAlignment(datum_size));

        std::string header_value = GenerateRandomString(header_size, &rng);
        std::string datum_value = GenerateRandomString(datum_size, &rng);

        strncpy(header, header_value.c_str(), header_size);
        header_values[header] = header_value;

        if (datum_size)
        {
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