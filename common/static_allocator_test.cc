/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    static_allocator_test.cc

Abstract:

    Unit tests for static_allocator.h

--*/

extern "C" {
#include "common/static_allocator.h"
}

#include <set>
#include <vector>

#include "googletest/include/gtest/gtest.h"

TEST(StaticMemoryAllocatorTest, StaticMemoryAllocatorInitialize)
{
    STATIC_MEMORY_ALLOCATOR allocator;
    ASSERT_TRUE(StaticMemoryAllocatorInitialize(&allocator, 4));
    ASSERT_EQ(0u, PointerListGetSize(&allocator.chunks));
    ASSERT_EQ(4u, allocator.allocation_size);
    ASSERT_EQ(0u, allocator.current_size);

    StaticMemoryAllocatorDestroy(&allocator);
}

TEST(StaticMemoryAllocatorTest, StaticMemoryAllocatorAllocate)
{
    STATIC_MEMORY_ALLOCATOR allocator;
    ASSERT_TRUE(StaticMemoryAllocatorInitialize(&allocator, sizeof(int)));
    
    std::vector<int *> allocations;
    for (int i = 0; i < 1000; i++)
    {
        int *allocated;
        ASSERT_TRUE(
            StaticMemoryAllocatorAllocate(&allocator, (void **)&allocated));
        *allocated = i;
        allocations.push_back(allocated);
    }

    for (int i = 0; i < 1000; i++)
    {
        EXPECT_EQ(i, *allocations[i]);
    }

    StaticMemoryAllocatorDestroy(&allocator);
}

TEST(StaticMemoryAllocatorTest, StaticMemoryAllocatorFreeAll)
{
    STATIC_MEMORY_ALLOCATOR allocator;
    ASSERT_TRUE(StaticMemoryAllocatorInitialize(&allocator, sizeof(int)));
    
    std::set<int *> allocations;
    for (int i = 0; i < 1000; i++)
    {
        int *allocated;
        ASSERT_TRUE(
            StaticMemoryAllocatorAllocate(&allocator, (void **)&allocated));
        allocations.insert(allocated);
    }

    StaticMemoryAllocatorFreeAll(&allocator);

    std::vector<int *> allocations_in_order;
    for (int i = 0; i < 1000; i++)
    {
        int *allocated;
        ASSERT_TRUE(
            StaticMemoryAllocatorAllocate(&allocator, (void **)&allocated));
        EXPECT_EQ(1u, allocations.erase(allocated));
        *allocated = i;
        allocations_in_order.push_back(allocated);
    }

    EXPECT_TRUE(allocations.empty());

    for (int i = 1000; i < 2000; i++)
    {
        int *allocated;
        ASSERT_TRUE(
            StaticMemoryAllocatorAllocate(&allocator, (void **)&allocated));
        *allocated = i;
        allocations_in_order.push_back(allocated);
    }

    for (int i = 0; i < 2000; i++)
    {
        EXPECT_EQ(i, *allocations_in_order[i]);
    }

    StaticMemoryAllocatorDestroy(&allocator);
}