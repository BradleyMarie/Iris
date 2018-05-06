/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pointer_list_test.cc

Abstract:

    Unit tests for pointer_list.h

--*/

extern "C" {
#include "common/pointer_list.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(PointerListTest, PointerListInitializeAndDestroy)
{
    POINTER_LIST list;
    list.list = NULL;
    list.capacity = 0;
    list.size = 0;

    ASSERT_TRUE(PointerListInitialize(&list));
    EXPECT_TRUE(list.list != NULL);
    EXPECT_NE(0u, list.capacity);
    EXPECT_EQ(0u, list.size);

    PointerListDestroy(&list);
    EXPECT_TRUE(list.list == NULL);
    EXPECT_EQ(0u, list.capacity);
    EXPECT_EQ(0u, list.size);
}

TEST(PointerListTest, PointerListAddAndRetrieve)
{
    POINTER_LIST list;
    ASSERT_TRUE(PointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(PointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ASSERT_EQ(1000u, PointerListGetSize(&list));

    for (size_t i = 0; i < 1000; i++) {
        const void *ptr = PointerListRetrieveAtIndex(&list, i);
        EXPECT_EQ(i, (size_t)(uintptr_t)ptr);
    }

    PointerListDestroy(&list);
}

TEST(PointerListTest, PointerListPrepareToAddPointers)
{
    POINTER_LIST list;
    ASSERT_TRUE(PointerListInitialize(&list));

    size_t required_capacity;
    ASSERT_TRUE(CheckedAddSizeT(list.capacity, 1, &required_capacity));

    ASSERT_TRUE(PointerListAddPointer(&list, NULL));
    ASSERT_TRUE(PointerListPrepareToAddPointers(&list, list.capacity));
    EXPECT_LE(required_capacity, list.capacity);
    size_t new_capacity = list.capacity;

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(PointerListPrepareToAddPointers(&list, 1));
        EXPECT_EQ(new_capacity, list.capacity);
    }

    PointerListDestroy(&list);
}

TEST(PointerListTest, PointerListClear)
{
    POINTER_LIST list;
    ASSERT_TRUE(PointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(PointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ASSERT_EQ(1000u, PointerListGetSize(&list));

    PointerListClear(&list);
    ASSERT_EQ(0u, PointerListGetSize(&list));

    PointerListDestroy(&list);
}

int Reverse(const void* left, const void* right) {
    const void **left_entry = (const void **)left;
    const void **right_entry = (const void **)right;

    if ((uintptr_t)*left_entry > (uintptr_t)*right_entry) {
        return -1;
    }

    return 1;
}

TEST(PointerListTest, PointerListSort)
{
    POINTER_LIST list;
    ASSERT_TRUE(PointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(PointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    PointerListSort(&list, Reverse);

    for (size_t i = 0; i < 1000; i++) {
        const void *ptr = PointerListRetrieveAtIndex(&list, 999 - i);
        EXPECT_EQ(i, (size_t)(uintptr_t)ptr);
    }

    PointerListDestroy(&list);
}